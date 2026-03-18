# LotBot v1.0

An XBoard-compatible chess engine written in C++, featuring minimax search with alpha-beta pruning and dynamic depth scaling.

**Contributors:** Robert Draganoiu, Liviu Enache, Vlad Sandu

## Build Instructions

```bash
make build   # compiles the executable (lot_bot)
make run     # runs the executable
make clean   # removes the executable
make check   # runs 10 games on xboard against fairymax at depth 2
```

## Architecture

The engine is organized into 5 layers, each building on top of the previous one.

### Layer 1 — Pieces (`pieces.h` / `pieces.cpp`)

Defines the piece abstractions used on the board. Each piece is an instance of a class extending the base `Piece` class, holding information about its position, material value, side (color), and type. This layer provides a clean object model that the board layer relies on.

### Layer 2 — Board (`board.h` / `board.cpp`)

Implements the chess rules through the `Board` class. All move validation and execution logic lives here.

| Method | Description |
|--------|-------------|
| `check_all_moves` | Generates all legal moves for a given side by iterating over remaining pieces and filtering out moves that would leave the king in check. |
| `move_piece` | Executes a move given source and destination coordinates. Handles special moves (promotion, castling, en passant). Does not validate legality — that is done upstream. |
| `undo_move` | Reverses the last move, restoring the board to its previous state. |

### Layer 3 — Engine / Search (`lot_bot.h` / `lot_bot.cpp`)

Implements the minimax search algorithm through the `LotBot` class. Uses the board to generate legal moves and evaluate positions.

| Method | Description |
|--------|-------------|
| `usermove` | Receives a move command from the layer above, parses it into internal board coordinates, and updates the board with the opponent's move. |
| `botmove` | Receives the signal to move, runs the minimax search, and returns the resulting move formatted for XBoard. |
| `minimax` | Implements minimax with alpha-beta pruning to find the best move in the current position. |
| `evaluate` | Performs a static evaluation of the board, producing a score that max layers try to maximize and min layers try to minimize. |

### Layer 4 — XBoard Communication (`engine_driver.h` / `engine_driver.cpp`)

Handles the protocol interface between XBoard and the engine through the `EngineDriver` class. Communication is split into two phases:

- **`board_setup`** — handles `new` and `protover` commands, ensuring the game starts correctly.
- **`game_loop`** — manages side switching in response to `black`, `white`, `force`, `new`, and `go` commands; signals the bot on its turn and sends the computed move to XBoard; forwards the opponent's move to the internal board; resets the bot when the current game ends.

### Layer 5 — Entry Point (`main.cpp`)

Instantiates `EngineDriver` and calls its `run` method.

> `utils.h` contains shared definitions used throughout the program, such as type enums and evaluation matrices.

## Algorithmic Approach

### Internal Move Representation

Moves on the internal board are managed efficiently using a hash map and two stacks:

- **`pieces_status`** — a hash map storing piece memory addresses and their alive/dead status.
- **`moves`** — a stack of `MoveInfo` structs (defined in `board.h`), each recording source/destination coordinates, a reference to the moved piece, and whether a capture occurred. When a piece moves, a `MoveInfo` is pushed onto the stack so that `undo_move` can pop it and reverse the operation.
- **`dead_pieces`** — a stack of references to captured pieces, maintained in parallel with the `moves` stack. When a capture happens, the captured piece's address is pushed here so that `undo_move` can immediately restore it to its position.

### Evaluation Function

The static evaluation is based on two components:

**Material values** — piece values are set relative to one another, similar to standard chess literature, with minor adjustments to bias preservation of certain pieces. The evaluation considers the bot's position stronger the larger the material advantage over the opponent.

**Positional values** — computed via evaluation matrices that add or subtract from a piece's base value depending on its board position. For example, a centralized knight is worth more than one on the rim because it controls more important squares and has more available moves. The engine carries two sets of matrices — one for the opening and one for the mid/endgame.

**Early termination** — if either side is in checkmate or stalemate, the evaluation returns immediately with an extreme score reflecting the outcome.

### Minimax Search

- Uses **alpha-beta pruning** and can compute depth-4 searches in under one second with a full board (benchmarked on an Intel Core i7-4720HQ @ 2.60 GHz).
- Moves are **ordered by likelihood of being chosen**, maximizing the effectiveness of alpha-beta cutoffs — this allows depth-6 searches in endgame positions without difficulty.
- **Dynamic depth scaling:** uses depth 4 until at least half the pieces (including queens) have been captured, then switches to depth 6. If fewer than 8 pieces remain, depth 6 is used regardless of piece types.

## References

- [chessprogramming.org](https://www.chessprogramming.org/) — articles on chess engine development
