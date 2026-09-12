// Prevents additional console window on Windows in release, DO NOT REMOVE!!
#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

#[cfg(target_os = "windows")]
#[no_mangle]
#[used]
pub static NvOptimusEnablement: i32 = 1; // Ensures NVIDIA dedicated GPU is used for rendering

fn main() {
    cgl_tauri_lib::run()
}
