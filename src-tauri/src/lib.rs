use std::time::{SystemTime, UNIX_EPOCH};

pub mod app;
pub mod renderer;

pub use app::run;

#[tauri::command]
fn set_clear_color(timestamp: i64) { // Function for handling clicks from the frontend
    let now = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .expect("time went backwards")
        .as_millis() as i64;

    let elapsed = now - timestamp;
    println!("Response time: {} ms", elapsed); // Prints latency (highly inaccurate as minimum unit with timesatmps is ms)
}