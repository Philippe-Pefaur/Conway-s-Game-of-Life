use std::collections::HashSet;
use std::fmt;

use crate::core::cells::{CellMap, CellState, MapError};

/// Controls cgl game logic.
pub struct GameController {
    active_pos: HashSet<(i32, i32)>, // positions which were affected by the last simulation step.
    to_be_killed: HashSet<(i32, i32)>, // positions queued for killing.
    to_be_revived: HashSet<(i32, i32)>, // position queued for revivign.
    pub cell_map: CellMap,
}

/// Defines *__GameController__* operations that can fail.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ControllerOperation {
    QueueCells,
    DrawCells,
    GetCell,
}

#[derive(Debug, Clone)]
pub enum ControllerError {
    OperationFailed {
        operation: ControllerOperation,
        source: MapError,
    },
}

impl GameController {
    /// Instances a new *__GameController__*.
    ///
    /// `chunk_size` defines the side lenght of every *__CellChunk__*
    pub fn new(chunk_size: usize) -> Self {
        Self {
            active_pos: HashSet::new(),
            to_be_killed: HashSet::new(),
            to_be_revived: HashSet::new(),
            cell_map: CellMap::new(chunk_size),
        }
    }

    /// Advances simulation by one step.
    pub fn step(&mut self) {
        println!("Instanced chunks: {}\n", self.cell_map.chunk_count());
        self.queue_cells(); // determine state changes
        self.active_pos.clear();

        self.update_cells(); // execute changes
        self.to_be_killed.clear();
        self.to_be_revived.clear();
    }

    /// Forces the modification of states in the simulation
    pub fn draw(&mut self, x: i32, y: i32, state: CellState) {
        set_cell(&mut self.cell_map, x, y, state);
        queue_affected(x, y, &mut self.active_pos, &mut self.cell_map); // queues affected cells as active
    }

    /// Cheks neighbouring cells for every position queued in `active_pos` and determines if its corresponding *__CellChunk__* should be killed, revived or ignored.
    ///
    /// ### Panic:
    ///
    /// Function provokes process to panic whenever a *__CellMap__* level operation fails.
    fn queue_cells(&mut self) {
        for &(x, y) in self.active_pos.iter() {
            // iterate through every active position
            match self.cell_map.get_cell(x, y) {
                Ok(cell) => {
                    // if cell is instanced manage both live and dead cases
                    let is_live = cell.is_live();
                    let neighbour_sum = self.neighbour_sum(x, y); // number of live neighbours

                    if is_live && !matches!(neighbour_sum, 2 | 3) {
                        enqueue((x, y), &mut self.to_be_killed);
                    } else if !is_live && neighbour_sum == 3 {
                        enqueue((x, y), &mut self.to_be_revived);
                    }
                }
                Err(MapError::ChunkNotFound { .. }) => {
                    // otherwise assume dead case
                    let neighbour_sum = self.neighbour_sum(x, y); // number of live neighbours
                    if neighbour_sum == 3 {
                        enqueue((x, y), &mut self.to_be_revived);
                    }
                }
                Err(error) => panic!("FATAL: {}", error), // upon operation faliure logic panics
            }
        }
    }

    /// Revives all cells at positions queued in `to_be_revived` and kills cells at positions queued in `to_be_killed`.
    ///
    /// Queues every position corresponding to a *__Cell__* potentially affected by the state changes.
    fn update_cells(&mut self) {
        for &(x, y) in &self.to_be_killed {
            set_cell(&mut self.cell_map, x, y, CellState::Dead);
            queue_affected(x, y, &mut self.active_pos, &mut self.cell_map);
        }
        for &(x, y) in &self.to_be_revived {
            set_cell(&mut self.cell_map, x, y, CellState::Alive);
            queue_affected(x, y, &mut self.active_pos, &mut self.cell_map);
        }
    }

    /// Count of live cells around the target `x`, `y` global position.
    ///
    /// `x`, `y` position coordinates follow a Cartisian style plane.
    fn neighbour_sum(&self, x: i32, y: i32) -> i32 {
        self.cell_map
            .neighbour_cells(x, y)
            .iter()
            .filter_map(|option| option.as_ref())
            .filter(|cell| cell.is_live())
            .count() as i32
    }
}

/// Changes the `state` of the *__Cell__* at the target `x`, `y` position.
///
/// ### Panic:
///
/// Function provokes process to panic whenever a *__CellMap__* level operation fails.
///
/// ### Note:
///
/// `x`, `y` position coordinates follow a Cartisian style plane.
fn set_cell(cell_map: &mut CellMap, x: i32, y: i32, state: CellState) {
    cell_map
        .set_cell(x, y, state)
        .map_err(|source| ControllerError::OperationFailed {
            operation: ControllerOperation::DrawCells,
            source,
        })
        .unwrap_or_else(|error| panic!("FATAL: {}", error));
}

/// Inserts a given position into a given queue.
///
/// As controller queues are of type *__HashSet__* positions already queued are not duplicated while conserving constant time complexity.
fn enqueue(pos: (i32, i32), queue: &mut HashSet<(i32, i32)>) {
    queue.insert(pos); // duplicated values are ignored
}

/// Queues the neighbor positions of a given target `x`, `y` position and queues them into a given queue.
///
/// Every neighbour position is considered affected on a state change as these have their live neighbour count modified by +- 1.
fn queue_affected(x: i32, y: i32, queue: &mut HashSet<(i32, i32)>, map: &mut CellMap) {
    enqueue((x, y), queue);
    for &pos in map.neighbour_pos(x, y).iter() {
        enqueue(pos, queue);
    }
}

impl fmt::Display for ControllerError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            ControllerError::OperationFailed { operation, source } => {
                write!(
                    f,
                    "GameController | operation {:?}\n    {}",
                    operation, source
                )
            }
        }
    }
}

impl std::error::Error for ControllerError {
    fn source(&self) -> Option<&(dyn std::error::Error + 'static)> {
        match self {
            ControllerError::OperationFailed { source, .. } => Some(source),
        }
    }
}
