use super::cell_state::CellState;

/// Unit that controls a *__CellState__*.
#[derive(Clone, Copy, Debug)]
pub struct Cell {
    state: CellState,
}

impl Cell {
    pub fn new(state: CellState) -> Self {
        Self { state }
    }

    pub fn set_state(&mut self, state: CellState) -> bool {
        let changed = self.state != state;
        if changed {
            self.state = state;
        }
        changed
    }

    pub fn is_live(&self) -> bool {
        self.state.is_live()
    }

    pub fn state(&self) -> CellState {
        self.state
    }
}

impl Default for Cell {
    /// Creates a new *__Cell__* with *__Dead__* as its `state`.
    fn default() -> Self {
        Self {
            state: CellState::Dead,
        }
    }
}
