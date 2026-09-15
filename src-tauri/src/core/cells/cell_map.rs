use std::collections::{HashMap, HashSet, VecDeque};
use std::fmt;

use crate::core::cells::{Cell, CellChunk, CellState, ChunkError};

/// Controls access to *__Cell__* instances through global coordinates.
///
/// Allows for accessing into uninstanced cells by instancing a new *__CellChunk__* if data persistance is required or handing dummy values when not.
///
/// The *__CellMap__* consists of square `chunk_size` side length chunks.
///
/// ### Note:
///
/// Only one origin *__CellChunk__* is instanced upon creation at chunk_position (0, 0).
pub struct CellMap {
    chunk_size: usize,
    cells: HashMap<(i32, i32), CellChunk>,
}

#[derive(Debug, Clone)]
pub enum MapError {
    /// An attempt was made to access an uninstanced chunk.
    ///
    /// ### Note:
    ///
    /// Not fatal.
    ChunkNotFound { x: i32, y: i32 },

    /// An operation failed at *__CellChunk__* level.
    ///
    /// ### Note:
    ///
    /// **Fatal**.
    OperationFailed { x: i32, y: i32, source: ChunkError },
}

impl CellMap {
    /// Instances a new *__CellMap__* with an origin *__CellChunk__* at chunk_position (0, 0).
    ///
    /// `chunk_size` defines the side lenght of every *__CellChunk__*
    ///
    /// ## Note:
    ///
    /// Only one *__CellChunk__* is instanced upon creation.
    pub fn new(chunk_size: usize) -> Self {
        assert!(
            chunk_size >= 1,
            "FATAL: CellMap | Attempted to create a CellMap of chunk_size = 0"
        );

        Self {
            chunk_size,
            cells: HashMap::from([
                ((0, 0), CellChunk::new(chunk_size)), // origin chunk
            ]),
        }
    }

    /// Returns the *__CellChunk__* to which the `x`, `y` global position belongs.
    ///
    /// ### Error:
    ///
    /// Returns an error whenever the targeted *__CellChunk__* is not instanced.
    ///
    /// ### Note:
    ///
    /// `x`, `y` position coordinates follow a Cartisian style plane.
    pub fn check_chunk(&self, x: i32, y: i32) -> Result<&CellChunk, MapError> {
        let (chunk_x, chunk_y) = self.chunk_coords(x, y); // transform global coordinates into chunk_position coordinates
        self.cells
            .get(&(chunk_x, chunk_y))
            .ok_or(MapError::ChunkNotFound { x, y })
    }

    /// Returns the *__CellChunk__* to which the `x`, `y` global position belongs.
    ///
    /// If the targeted *__CellChunk__* doesn't exist it is instanced.
    ///
    /// ### Note:
    ///
    /// `x`, `y` position coordinates follow a Cartisian style plane.
    fn chunk_at(&mut self, x: i32, y: i32) -> &mut CellChunk {
        let (chunk_x, chunk_y) = self.chunk_coords(x, y);
        self.cells
            .entry((chunk_x, chunk_y))
            .or_insert_with(|| CellChunk::new(self.chunk_size))
    }

    /// Returns the *__Cell__* at `x`, `y` global position.
    ///
    /// ### Error:
    ///
    /// Returns an error whenever the targeted cell's *__CellChunk__* is not instanced.
    ///
    /// ### Note:
    ///
    /// `x`, `y` position coordinates follow a Cartisian style plane.
    pub fn get_cell(&self, x: i32, y: i32) -> Result<&Cell, MapError> {
        let chunk = self.check_chunk(x, y)?; // propagates the error if chunk doesn't exist
        let (x_pos, y_pos) = self.in_chunk_pos(x, y); // transform global position into local position

        Ok(
            chunk
                .get_cell(x_pos, y_pos)
                .map_err(|error| MapError::OperationFailed {
                    x,
                    y,
                    source: error,
                })?, // propagates the error if operation fails
        )
    }

    /// Changes the `state` of the *__Cell__* at `x`, `y` global position.
    ///
    /// If the targeted cell's *__CellChunk__* doesn't exist it is instanced.
    ///
    /// ### Error:
    ///
    /// Returns an error if *__CellChunk__* level operation fails.
    ///
    /// ### Note:
    ///
    /// `x`, `y` position coordinates follow a Cartisian style plane.
    pub fn set_cell(&mut self, x: i32, y: i32, state: CellState) -> Result<(), MapError> {
        let size = self.chunk_size as i32;
        let x_pos = x.rem_euclid(size) as usize;
        let y_pos = y.rem_euclid(size) as usize;

        let is_empty = {
            let chunk = self.chunk_at(x, y);
            chunk
                .set_state(x_pos, y_pos, state)
                .map_err(|error| MapError::OperationFailed {
                    x,
                    y,
                    source: error,
                })?;
            chunk.is_empty()
        };

        let (chunk_x, chunk_y) = self.chunk_coords(x, y);
        if is_empty {
            self.cleanup_chunks(chunk_x, chunk_y);
        }

        Ok(())
    }

    /// Returns an array of all 8 neighbouring positions to a `x`, `y` global position.
    ///
    /// ### Note:
    ///
    /// `x`, `y` position coordinates follow a Cartisian style plane.
    pub fn neighbour_pos(&self, x: i32, y: i32) -> [(i32, i32); 8] {
        let mut neighbours: [(i32, i32); 8] = [(0, 0); 8];
        let mut i = 0;

        // Iterate thorugh neighbouring positions
        for x_offset in -1..=1 {
            for y_offset in -1..=1 {
                if x_offset == 0 && y_offset == 0 {
                    // skip origin position
                    continue;
                }

                neighbours[i] = (x + x_offset, y + y_offset);

                i += 1;
            }
        }

        neighbours
    }

    /// Returns an array of all 8 neighbouring cells to a *__Cell__* at `x`, `y` global position.
    ///
    /// ### Error:
    ///
    /// If the targeted *__Cell__* belongs to an uninstanced *__CellChunk__*, None is returned instead
    ///
    /// ### Note:
    ///
    /// `x`, `y` position coordinates follow a Cartisian style plane.
    pub fn neighbour_cells(&self, x: i32, y: i32) -> [Option<&Cell>; 8] {
        let mut neighbours: [Option<&Cell>; 8] = [None; 8];

        // Iterate through neighbors and attempt to obtain the corresponding cell
        for (i, &(neighbour_x, neighbour_y)) in self.neighbour_pos(x, y).iter().enumerate() {
            neighbours[i] = match self.get_cell(neighbour_x, neighbour_y) {
                Ok(cell) => Some(cell),
                Err(MapError::ChunkNotFound { .. }) => None, // hands None when cell belongs to an uninstanced chunk
                Err(error) => panic!("FATAL: {}", error),
            };
        }

        neighbours
    }

    /// Transforms a given `x`, `y` global position into `chunk_x`, `chunk_y` global chunk coordinates
    ///
    /// ### Note:
    ///
    /// `x`, `y`, `chunk_x`, `chunk_y` global coordinates follow a Cartisian style plane.
    pub fn chunk_coords(&self, x: i32, y: i32) -> (i32, i32) {
        let size = self.chunk_size as i32;

        (Self::div_floor(x, size), Self::div_floor(y, size)) // rounds down to the preceding integer value while preserving sign
    }

    pub fn chunk_count(&self) -> usize {
        self.cells.iter().count()
    }

    /// Removes the *__CellChunk__* containing `chunk_x`, `chunk_y` at global chunk position from the `cells` *__HashMap__*.
    ///
    /// The *__CellChunk__* is destroyed when dropped.
    ///
    /// ### Note:
    ///
    /// `chunk_x`, `chunk_y` global chunk coordinates follow a Cartisian style plane.
    fn drop_chunk(&mut self, chunk_x: i32, chunk_y: i32) -> bool {
        self.cells.remove(&(chunk_x, chunk_y)).is_some()
    }

    fn cleanup_chunks(&mut self, start_x: i32, start_y: i32) {
        let mut queue = VecDeque::new();
        let mut visited = HashSet::new();

        queue.push_back((start_x, start_y));
        visited.insert((start_x, start_y));

        while let Some((chunk_x, chunk_y)) = queue.pop_front() {
            let chunk = match self.cells.get(&(chunk_x, chunk_y)) {
                Some(chunk) => chunk,
                None => continue,
            };

            if !chunk.is_empty() {
                continue;
            }

            let has_live_neighbour = self
                .neighbour_pos(chunk_x, chunk_y)
                .iter()
                .filter_map(|pos| self.cells.get(pos))
                .any(|neighbour| !neighbour.is_empty());

            if !has_live_neighbour {
                self.drop_chunk(chunk_x, chunk_y);
            }

            for (new_x, new_y) in self.neighbour_pos(chunk_x, chunk_y) {
                if visited.insert((new_x, new_y)) {
                    queue.push_back((new_x, new_y));
                }
            }
        }
    }

    /// Transforms a given `x`, `y` global position into local chunk coordinates
    ///
    /// ### Note:
    ///
    /// `x`, `y` global coordinates follow a Cartisian style plane.
    fn in_chunk_pos(&self, x: i32, y: i32) -> (usize, usize) {
        let size = self.chunk_size as i32;
        (x.rem_euclid(size) as usize, y.rem_euclid(size) as usize) // rem_euclid correctly transform negative signed remeinders by wraping around to the end of the chunks positive axis
    }

    /// Rounds down to the preceding integer value including negative signs
    fn div_floor(num: i32, div: i32) -> i32 {
        if num >= 0 || num % div == 0 {
            // if number is positive or division results in an integer no transformation is required
            num / div
        } else {
            // negative signed numbers are truncated towards 0 instead of the preceding negative value
            (num / div) - 1 // rational negative divisions are offset one unit towards the negative values.
        }
    }
}

impl fmt::Display for MapError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            MapError::ChunkNotFound { x, y } => {
                write!(f, "CellMap | chunk at ({}, {}) was not found", x, y)
            }
            MapError::OperationFailed { x, y, source } => {
                write!(f, "CellMap | error at ({}, {}):\n    {}", x, y, source)
            }
        }
    }
}

impl std::error::Error for MapError {
    fn source(&self) -> Option<&(dyn std::error::Error + 'static)> {
        match self {
            MapError::OperationFailed { source, .. } => Some(source),
            _ => None,
        }
    }
}
