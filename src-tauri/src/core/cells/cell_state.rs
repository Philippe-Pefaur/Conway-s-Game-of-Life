/// Indicates if a *__Cell__* is dead or alive.
///
/// ### Note:
///
/// Meant to be used for game logic.
#[repr(u8)]
#[derive(Copy, Clone, Debug, PartialEq)]
pub enum CellState {
    Dead = 0,
    Alive = 1,
}

impl CellState {
    /// Generate a *__boolean__* value from a *__CellState__*.
    pub fn is_live(self) -> bool {
        self == CellState::Alive
    }

    /// Generate a *__CellState__* from a *__boolean__* value.
    pub fn from_bool(alive: bool) -> Self {
        if alive {
            CellState::Alive
        } else {
            CellState::Dead
        }
    }
}
