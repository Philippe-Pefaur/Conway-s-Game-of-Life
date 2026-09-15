use std::fmt;
use std::ops::{Index, IndexMut};

use super::{Cell, CellState};

/// A grid of multiple *__Cell__* managed with local coordiantes.
///
/// The grid consists of a square where the size are `size` length.
///
/// ### Note:
///
/// Every *__Cell__* in the grid is instanced upon creation.
#[derive(Debug, Clone)]
pub struct CellChunk {
    size: usize,
    live_count: u32,
    cells: Vec<Cell>,
}

#[derive(Debug, Clone)]
pub enum ChunkError {
    /// An attempt was made to acces a *__Cell__* outside of the chunks local boundaries.
    OutOfBounds { x: usize, y: usize },
}

impl CellChunk {
    /// Instances a square grid of *__Cell__* with side length of `size`.
    ///
    /// ### Note:
    ///
    /// Every *__Cell__* is instanced immediately as *__Dead__*.
    pub fn new(size: usize) -> Self {
        Self {
            size,
            live_count: 0,
            cells: vec![Cell::default(); size * size],
        }
    }

    /// Returns the flat `index` of a position.
    ///
    /// ### Errors:
    ///
    /// Retruns an error if the position given is out of bounds.
    ///
    /// ### Note:
    ///
    /// `x`, `y` position coordinates follow a Cartisian style plane.
    pub fn index_of(&self, x: usize, y: usize) -> Result<usize, ChunkError> {
        if x >= self.size || y >= self.size {
            return Err(ChunkError::OutOfBounds { x, y });
        }

        Ok(x + ((self.size - 1 - y) * self.size)) // convert coordinates while inverting y axis.
    }

    /// Returns *__Cell__* at local position `x`, `y`.
    ///
    /// ### Note:
    ///
    /// `x`, `y` position coordinates follow a Cartisian style plane.
    pub fn get_cell(&self, x: usize, y: usize) -> Result<&Cell, ChunkError> {
        let index = self.index_of(x, y)?;
        Ok(&self.cells[index])
    }

    /// Modifies the state of the *__Cell__* at local position `x`, `y`.
    ///
    /// ### Errors:
    ///
    /// Retruns an error if the position given is out of bounds.
    ///
    /// ### Note:
    ///
    /// `x`, `y` position coordinates follow a Cartisian style plane.
    pub fn set_state(&mut self, x: usize, y: usize, state: CellState) -> Result<(), ChunkError> {
        let index = self.index_of(x, y)?; // propagates error if position is out of bounds
        if self.cells[index].set_state(state) {
            if state == CellState::Alive {
                self.live_count += 1;
            } else {
                self.live_count -= 1;
            }
        }
        Ok(())
    }

    /// True if *__CellChunk__* has no live cells
    pub fn is_empty(&self) -> bool {
        self.live_count == 0
    }
}

// Index allows for access to cells through index iteration
impl Index<usize> for CellChunk {
    type Output = Cell;

    fn index(&self, index: usize) -> &Self::Output {
        &self.cells[index]
    }
}

impl IndexMut<usize> for CellChunk {
    fn index_mut(&mut self, index: usize) -> &mut Self::Output {
        &mut self.cells[index]
    }
}

impl fmt::Display for ChunkError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            ChunkError::OutOfBounds { x, y } => {
                write!(
                    f,
                    "CellChunk | operation failed on ({}, {}) as found out of bounds",
                    x, y
                )
            }
        }
    }
}

impl std::error::Error for ChunkError {}
