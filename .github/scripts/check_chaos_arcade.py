#!/usr/bin/env python3
"""Exercise the real C checkers engine, including legal capture chains and AI."""
import ctypes as C
import pathlib, subprocess, tempfile
ROOT = pathlib.Path(__file__).resolve().parents[2]
class Board(C.Structure):
    _fields_ = [('squares', C.c_uint8 * 32), ('turn', C.c_uint8), ('quietTurns', C.c_uint16)]
class Move(C.Structure):
    _fields_ = [('path', C.c_uint8 * 13), ('length', C.c_uint8), ('captures', C.c_uint8)]
with tempfile.TemporaryDirectory() as folder:
    library = pathlib.Path(folder) / 'checkers.so'
    subprocess.run(['gcc', '-std=c99', '-Wall', '-Wextra', '-Werror', '-shared', '-fPIC', '-O2', '-DCHAOS_CHECKERS_HOST', '-I'+str(ROOT/'include'), str(ROOT/'src/chaos_checkers.c'), '-o', str(library)], check=True)
    engine = C.CDLL(str(library))
    engine.ChaosCheckersMoves.argtypes = [C.POINTER(Board), C.POINTER(Move)]
    engine.ChaosCheckersApply.argtypes = [C.POINTER(Board), C.POINTER(Move)]
    engine.ChaosCheckersChoose.argtypes = [C.POINTER(Board), C.c_uint32, C.c_uint32, C.POINTER(Move)]
    def moves(board):
        result = (Move * 96)()
        count = engine.ChaosCheckersMoves(C.byref(board), result)
        return list(result)[:count]
    def key(move): return tuple(move.path[:move.length])
    def sq(row, col): return engine.ChaosCheckersSquare(row, col)
    board = Board(); engine.ChaosCheckersInit(C.byref(board))
    assert len(moves(board)) == 7
    assert sum(piece == 1 for piece in board.squares) == 12
    assert sum(piece == 2 for piece in board.squares) == 12
    # Capture is compulsory even when another piece has quiet moves.
    board = Board(); board.turn = 1
    board.squares[sq(6,1)] = 1; board.squares[sq(5,2)] = 2
    board.squares[sq(3,4)] = 2; board.squares[sq(6,5)] = 1
    legal = moves(board)
    assert len(legal) == 1 and key(legal[0]) == (sq(6,1), sq(4,3), sq(2,5))
    assert legal[0].captures == 2
    engine.ChaosCheckersApply(C.byref(board), C.byref(legal[0]))
    assert board.squares[sq(2,5)] == 1 and board.squares[sq(5,2)] == board.squares[sq(3,4)] == 0
    assert board.turn == 2
    # A man crowns and stops, rather than continuing backwards as a new king.
    board = Board(); board.turn = 1
    board.squares[sq(2,1)] = 1; board.squares[sq(1,2)] = 2; board.squares[sq(1,4)] = 2
    legal = moves(board)
    assert len(legal) == 1 and legal[0].length == 2
    engine.ChaosCheckersApply(C.byref(board), C.byref(legal[0]))
    assert board.squares[sq(0,3)] == 5 and board.squares[sq(1,4)] == 2
    # Existing kings may capture both backwards and forwards.
    board = Board(); board.turn = 1; board.squares[sq(4,3)] = 5; board.squares[sq(5,4)] = 2
    assert key(moves(board)[0]) == (sq(4,3), sq(6,5))
    # Terminal side without moves loses; the AI cannot invent a move.
    board = Board(); board.turn = 2
    chosen = Move(); assert engine.ChaosCheckersChoose(C.byref(board), 2, 123, C.byref(chosen)) == 0
    # Seeded full games check legal AI moves at every level, conservation,
    # promotion, capture priority and bounded game termination.
    for seed in range(30):
        board = Board(); engine.ChaosCheckersInit(C.byref(board))
        previous_count = 24
        for turn in range(2400):
            legal = moves(board)
            if not legal or board.quietTurns >= 80: break
            if any(m.captures for m in legal): assert all(m.captures for m in legal)
            chosen = Move()
            assert engine.ChaosCheckersChoose(C.byref(board), seed % 3, seed * 937 + turn, C.byref(chosen)) == 1
            assert key(chosen) in [key(m) for m in legal]
            engine.ChaosCheckersApply(C.byref(board), C.byref(chosen))
            count = sum(bool(piece) for piece in board.squares)
            assert count == previous_count - chosen.captures
            previous_count = count
            assert all(piece in (0,1,2,5,6) for piece in board.squares)
        else: raise AssertionError('game failed to finish within 2400 turns')
print('PASS checkers mandatory captures, multi-jumps, promotion, kings, terminal states and 30 complete legal AI games.')
