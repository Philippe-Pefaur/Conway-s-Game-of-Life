use std::sync::Mutex;

use tauri::WebviewWindow;

pub struct RenderState {
    pub surface: wgpu::Surface<'static>,
    pub device: wgpu::Device,
    pub queue: wgpu::Queue,
    pub config: wgpu::SurfaceConfiguration,

    pub full_screen_buffer: wgpu::Buffer,
    pub grid_data_buffer: wgpu::Buffer,
    pub full_screen_bind_group: wgpu::BindGroup,
    pub full_screen_pipeline: wgpu::RenderPipeline,
}

#[repr(u32)]
#[derive(Copy, Clone, Debug, PartialEq)]
pub enum CellState {
    Dead = 0,
    Alive = 1,
    Selected = 2,
}

unsafe impl bytemuck::Zeroable for CellState {}
unsafe impl bytemuck::Pod for CellState {}

#[repr(C)]
#[derive(Copy, Clone, Debug, bytemuck::Pod, bytemuck::Zeroable)]
pub struct FullScreenGridData {
    pub width: u32,
    pub height: u32,
    pub cell_size: u32,
    pub window_scale: f32,
}

pub struct StateGrid {
    width: u32,
    height: u32,
    cell_size: u32,
    window_scale: f32,
    cells: Vec<CellState>,
}

impl StateGrid {
    pub fn new(width: u32, height: u32, cell_size: u32, window_scale: f32) -> Self {
        Self {
            width,
            height,
            cell_size,
            window_scale,
            cells: vec![CellState::Dead; (width * height) as usize],
        }
    }

    pub fn index(&self, x: u32, y: u32) -> usize {
        (y * self.width + x) as usize
    }

    pub fn get(&self, x: u32, y: u32) -> CellState {
        self.cells[self.index(x, y)]
    }

    pub fn set(&mut self, x: u32, y: u32, state: CellState) {
        let idx = self.index(x, y);
        self.cells[idx] = state;
    }

    pub fn as_bytes(&self) -> &[u8] {
        bytemuck::cast_slice(&self.cells)
    }

    pub fn width(&self) -> u32 {
        self.width
    }
    pub fn height(&self) -> u32 {
        self.height
    }
    pub fn cell_size(&self) -> u32 {
        self.cell_size
    }
    pub fn window_scale(&self) -> f32 {
        self.window_scale
    }
}

// Entire screen is passed to the fragment shader deciding pixel color according to cell state grid
const FULL_SCREEN_SHADER_CODE: &str = r#"
    struct GridData {
        width: u32,
        height: u32,
        cell_size: u32,
        window_scale: f32
    };

    @group(0) @binding(0)
    var<uniform> grid_data: GridData;

    @group(0) @binding(1)
    var<storage, read> cell_states: array<u32>;

    @vertex
    fn vs_main(@builtin(vertex_index) vertex_index: u32) -> @builtin(position) vec4<f32> {
        // one big triangle that covers the whole clip space
        var pos = array<vec2<f32>, 3>(
            vec2<f32>(-1.0, -1.0),
            vec2<f32>( 3.0, -1.0),
            vec2<f32>(-1.0,  3.0),
        );
        return vec4<f32>(pos[vertex_index], 0.0, 1.0);
    }

    @fragment
    fn fs_main(@builtin(position) pos: vec4<f32>) -> @location(0) vec4<f32> {
        let width = grid_data.width;
        let height = grid_data.height;
        let cell_size = grid_data.cell_size;
        let window_scale = grid_data.window_scale;
        let x = u32(pos.x / window_scale / f32(cell_size));
        let y = height - 1 - u32(pos.y / window_scale / f32(cell_size));

        if (x >= width || y >= height) {
            return vec4<f32>(0.0, 0.0, 0.0, 1.0);
        }

        let idx = y * width + x;
        let state = cell_states[idx];

        switch state {
            case 0u { return vec4<f32>(0.0, 0.0, 0.0, 1.0); }
            case 1u { return vec4<f32>(1.0, 1.0, 1.0, 1.0); }
            case 2u { return vec4<f32>(0.3, 0.3, 0.3, 1.0); }
            default { return vec4<f32>(0.0, 0.0, 0.0, 1.0); }
        }
    }
"#;

pub async fn init_render_state<R: tauri::Runtime>(
    window: &WebviewWindow<R>,
    grid_size: u32,
) -> RenderState {
    let instance = wgpu::Instance::new(wgpu::InstanceDescriptor {
        backends: wgpu::Backends::PRIMARY,
        ..Default::default()
    });

    let surface = unsafe {
        let target =
            wgpu::SurfaceTargetUnsafe::from_window(window).expect("failed to get window handle");
        instance
            .create_surface_unsafe(target)
            .expect("failed to create surface")
    };

    let adapter = instance
        .request_adapter(&wgpu::RequestAdapterOptions {
            power_preference: wgpu::PowerPreference::HighPerformance,
            compatible_surface: Some(&surface),
            force_fallback_adapter: false,
        })
        .await
        .expect("no suitable GPU adapter found");

    let (device, queue) = adapter
        .request_device(
            &wgpu::DeviceDescriptor {
                label: Some("Conway Device"),
                required_features: wgpu::Features::empty(),
                required_limits: wgpu::Limits::default(),
                memory_hints: wgpu::MemoryHints::default(),
            },
            None,
        )
        .await
        .expect("failed to create device");

    let size = window.inner_size().expect("failed to get window size");
    let surface_caps = surface.get_capabilities(&adapter);
    let surface_format = surface_caps
        .formats
        .iter()
        .copied()
        .find(|f| f.is_srgb())
        .unwrap_or(surface_caps.formats[0]);

    let config = wgpu::SurfaceConfiguration {
        usage: wgpu::TextureUsages::RENDER_ATTACHMENT,
        format: surface_format,
        width: size.width.max(1),
        height: size.height.max(1),
        present_mode: wgpu::PresentMode::AutoNoVsync,
        desired_maximum_frame_latency: 2,
        alpha_mode: surface_caps.alpha_modes[0],
        view_formats: vec![],
    };

    surface.configure(&device, &config);

    let full_screen_shader = device.create_shader_module(wgpu::ShaderModuleDescriptor {
        label: Some("Full Screen Shader"),
        source: wgpu::ShaderSource::Wgsl(FULL_SCREEN_SHADER_CODE.into()),
    });

    let full_screen_bind_group_layout =
        device.create_bind_group_layout(&wgpu::BindGroupLayoutDescriptor {
            label: Some("Full Screen Bind Group Layout"),
            entries: &[
                // grid dimensions
                wgpu::BindGroupLayoutEntry {
                    binding: 0,
                    visibility: wgpu::ShaderStages::FRAGMENT,
                    ty: wgpu::BindingType::Buffer {
                        ty: wgpu::BufferBindingType::Uniform,
                        has_dynamic_offset: false,
                        min_binding_size: Some(
                            std::num::NonZeroU64::new(
                                std::mem::size_of::<FullScreenGridData>() as u64
                            )
                            .unwrap(),
                        ),
                    },
                    count: None,
                },
                // cell state array
                wgpu::BindGroupLayoutEntry {
                    binding: 1,
                    visibility: wgpu::ShaderStages::FRAGMENT,
                    ty: wgpu::BindingType::Buffer {
                        ty: wgpu::BufferBindingType::Storage { read_only: true },
                        has_dynamic_offset: false,
                        min_binding_size: None,
                    },
                    count: None,
                },
            ],
        });

    let full_screen_pipeline_layout =
        device.create_pipeline_layout(&wgpu::PipelineLayoutDescriptor {
            label: Some("Full Screen Pipeline Layout"),
            bind_group_layouts: &[&full_screen_bind_group_layout],
            push_constant_ranges: &[],
        });

    let full_screen_pipeline = device.create_render_pipeline(&wgpu::RenderPipelineDescriptor {
        label: Some("Full Screen Render Pipeline"),
        layout: Some(&full_screen_pipeline_layout),
        vertex: wgpu::VertexState {
            module: &full_screen_shader,
            entry_point: "vs_main",
            buffers: &[],
            compilation_options: wgpu::PipelineCompilationOptions::default(),
        },
        fragment: Some(wgpu::FragmentState {
            module: &full_screen_shader,
            entry_point: "fs_main",
            targets: &[Some(wgpu::ColorTargetState {
                format: config.format,
                blend: Some(wgpu::BlendState::REPLACE),
                write_mask: wgpu::ColorWrites::ALL,
            })],
            compilation_options: wgpu::PipelineCompilationOptions::default(),
        }),
        primitive: wgpu::PrimitiveState {
            topology: wgpu::PrimitiveTopology::TriangleList,
            ..Default::default()
        },
        depth_stencil: None,
        multisample: wgpu::MultisampleState::default(),
        multiview: None,
        cache: None,
    });

    let grid_data_buffer = device.create_buffer(&wgpu::BufferDescriptor {
        label: Some("Grid Data Buffer"),
        size: std::mem::size_of::<FullScreenGridData>() as wgpu::BufferAddress,
        usage: wgpu::BufferUsages::UNIFORM | wgpu::BufferUsages::COPY_DST,
        mapped_at_creation: false,
    });

    // enough for one cell per physical pixel; recreate this on resize if needed
    let max_buffer_size = grid_size as u64 * std::mem::size_of::<CellState>() as u64;

    let full_screen_buffer = device.create_buffer(&wgpu::BufferDescriptor {
        label: Some("Full Screen Buffer"),
        size: max_buffer_size,
        usage: wgpu::BufferUsages::STORAGE | wgpu::BufferUsages::COPY_DST,
        mapped_at_creation: false,
    });

    let full_screen_bind_group = device.create_bind_group(&wgpu::BindGroupDescriptor {
        label: Some("Full Screen Bind Group"),
        layout: &full_screen_bind_group_layout,
        entries: &[
            wgpu::BindGroupEntry {
                binding: 0,
                resource: grid_data_buffer.as_entire_binding(),
            },
            wgpu::BindGroupEntry {
                binding: 1,
                resource: full_screen_buffer.as_entire_binding(),
            },
        ],
    });

    RenderState {
        surface,
        device,
        queue,
        config,
        full_screen_buffer,
        grid_data_buffer,
        full_screen_bind_group,
        full_screen_pipeline,
    }
}

fn acquire_frame(state: &RenderState) -> Option<wgpu::SurfaceTexture> {
    match state.surface.get_current_texture() {
        Ok(output) => Some(output),
        Err(wgpu::SurfaceError::Lost | wgpu::SurfaceError::Outdated) => {
            state.surface.configure(&state.device, &state.config);
            None
        }
        Err(wgpu::SurfaceError::Timeout) => None,
        Err(e) => {
            eprintln!("Surface error: {:?}", e);
            None
        }
    }
}

pub fn clear_screen(state: &Mutex<RenderState>, clear_color: wgpu::Color) {
    let state = state.lock().unwrap();
    let output = match acquire_frame(&state) {
        Some(o) => o,
        None => return,
    };

    let view = output
        .texture
        .create_view(&wgpu::TextureViewDescriptor::default());

    let mut encoder = state
        .device
        .create_command_encoder(&wgpu::CommandEncoderDescriptor {
            label: Some("Clear Encoder"),
        });

    {
        let _render_pass = encoder.begin_render_pass(&wgpu::RenderPassDescriptor {
            label: Some("Clear Render Pass"),
            color_attachments: &[Some(wgpu::RenderPassColorAttachment {
                view: &view,
                resolve_target: None,
                ops: wgpu::Operations {
                    load: wgpu::LoadOp::Clear(clear_color),
                    store: wgpu::StoreOp::Store,
                },
            })],
            depth_stencil_attachment: None,
            occlusion_query_set: None,
            timestamp_writes: None,
        });
    }

    state.queue.submit(std::iter::once(encoder.finish()));
    output.present();
}

pub fn render_screen(state: &Mutex<RenderState>, grid: &StateGrid) {
    let state = state.lock().unwrap();
    let output = match acquire_frame(&state) {
        Some(o) => o,
        None => return,
    };

    // upload grid dimensions
    let grid_data = FullScreenGridData {
        width: grid.width(),
        height: grid.height(),
        cell_size: grid.cell_size(),
        window_scale: grid.window_scale(),
    };

    state
        .queue
        .write_buffer(&state.grid_data_buffer, 0, bytemuck::bytes_of(&grid_data));

    // upload cell state vector
    state
        .queue
        .write_buffer(&state.full_screen_buffer, 0, grid.as_bytes());

    let view = output
        .texture
        .create_view(&wgpu::TextureViewDescriptor::default());

    let mut encoder = state
        .device
        .create_command_encoder(&wgpu::CommandEncoderDescriptor {
            label: Some("Full Screen Frame Encoder"),
        });

    {
        let mut render_pass = encoder.begin_render_pass(&wgpu::RenderPassDescriptor {
            label: Some("Full Screen Render Pass"),
            color_attachments: &[Some(wgpu::RenderPassColorAttachment {
                view: &view,
                resolve_target: None,
                ops: wgpu::Operations {
                    load: wgpu::LoadOp::Clear(wgpu::Color::BLACK),
                    store: wgpu::StoreOp::Store,
                },
            })],
            depth_stencil_attachment: None,
            occlusion_query_set: None,
            timestamp_writes: None,
        });

        render_pass.set_pipeline(&state.full_screen_pipeline);
        render_pass.set_bind_group(0, &state.full_screen_bind_group, &[]);
        // one full-screen triangle, 3 vertices, 1 instance
        render_pass.draw(0..3, 0..1);
    }

    state.queue.submit(std::iter::once(encoder.finish()));
    output.present();
}
