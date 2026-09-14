// use std::sync::{Arc, Mutex};
use std::thread;
use std::time::{Duration, Instant};

use tauri::{App, Manager};

use crate::core::cells::CellState;
use crate::core::game_controller::GameController;
// use crate::shader::renderer::CellState::{self, Alive, Dead};
// use crate::shader::renderer::{RenderState, StateGrid, clear_screen, init_render_state, render_screen};

// const GRID_WIDTH: u32 = 128;
// const GRID_HEIGHT: u32 = 72;
// const CELL_SIZE: u32 = 10;

fn setup_renderer<R: tauri::Runtime>(
    // WPU initialization and render thread creation
    app: &mut App<R>,
) -> Result<(), Box<dyn std::error::Error>> {
    // let window = app
    //     .get_webview_window("main")
    //     .ok_or("main window not found")?;

    // let window_scale:f32;

    // if (GRID_WIDTH == 1920) && (GRID_HEIGHT == 1080) {
    //     window_scale = 1.0;
    // } else {
    //     window_scale = window.scale_factor().unwrap_or(1.0) as f32;
    // }

    // let render_state = tauri::async_runtime::block_on(init_render_state(&window, GRID_WIDTH*GRID_HEIGHT));
    // app.manage(Arc::new(Mutex::new(render_state)));

    // let app_handle = app.handle().clone();

    // if let Some(state) = app_handle.try_state::<Arc<Mutex<RenderState>>>() {
    //     clear_screen(&state, wgpu::Color::BLACK);
    // }

    // let mut draw_state: CellState = Alive;

    // Test simulation with a three cell wide line instancing three chunks during simulation
    let mut game_controller = GameController::new(32);
    game_controller.draw(0, -1, CellState::Alive);
    game_controller.draw(0, 0, CellState::Alive);
    game_controller.draw(0, 1, CellState::Alive);

    thread::spawn(move || {
        loop {
            // Testing print into terminal
            let state_char = |x, y| -> char {
                match game_controller.cell_map.get_cell(x, y) {
                    Ok(cell) if cell.is_live() => 'O',
                    Ok(_) => '.',
                    Err(error) => '.',
                }
            };
            let map_print = format!(
                r#"
                {}, {}, {}
                {}, {}, {}
                {}, {}, {}
                "#,
                state_char(-1, 1),
                state_char(0, 1),
                state_char(1, 1),
                state_char(-1, 0),
                state_char(0, 0),
                state_char(1, 0),
                state_char(-1, -1),
                state_char(0, -1),
                state_char(1, -1),
            );
            println!("{}", map_print);

            game_controller.step();

            //thread::sleep(Duration::from_millis(2000)); // delay for changes observation
        }
    });

    // thread::spawn(move || { // render thread looping cell drawing for testing
    //     let mut frame_count = 0u32;
    //     let mut last_print = Instant::now();

    //     let mut current_cell: u32 = 0;

    //     let mut grid = StateGrid::new(GRID_WIDTH, GRID_HEIGHT, CELL_SIZE, window_scale);

    //     loop {
    //         grid.set(
    //             current_cell % GRID_WIDTH,
    //             current_cell / GRID_WIDTH,
    //             draw_state
    //             );

    //         // println!("current_cell: {}, {}\n", current_cell % GRID_WIDTH, current_cell / GRID_WIDTH); // q

    //         if let Some(state) = app_handle.try_state::<Arc<Mutex<RenderState>>>() {
    //             render_screen(&state, &grid);
    //         }

    //         frame_count += 1;

    //         let elapsed = last_print.elapsed();
    //         if elapsed >= Duration::from_secs(1) {
    //             let fps = frame_count as f64 / elapsed.as_secs_f64();
    //             println!("FPS: {:.2}", fps);
    //             frame_count = 0;
    //             last_print = Instant::now();
    //         }

    //         current_cell += 1;

    //         if current_cell == GRID_WIDTH*GRID_HEIGHT {
    //             current_cell = 0;
    //             if draw_state == Alive {
    //                 draw_state = Dead;
    //             } else {
    //                 draw_state = Alive;
    //             }
    //         }
    //     }
    // });

    Ok(())
}

pub fn run() {
    tauri::Builder::default()
        .plugin(tauri_plugin_opener::init())
        .invoke_handler(tauri::generate_handler![crate::set_clear_color])
        .setup(setup_renderer)
        .build(tauri::generate_context!())
        .expect("error building tauri application")
        .run(|_app_handle, _event| {});
}
