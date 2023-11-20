import pygame
import sys

# Initialize Pygame
pygame.init()

# Constants
WIDTH, HEIGHT = 600, 600
GRID_SIZE = 9
CELL_SIZE = WIDTH // GRID_SIZE

# Colors
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
GRAY = (200, 200, 200)
RED = (255, 0, 0)

# Example input string with some filled cells
sudoku_input = "004300209005009001070060043006002087190007400050083000600000105003508690042910300"

# Example list of colored cells (row, col)
colored_cells = [(0, 0), (1, 1), (2, 2), (3, 3), (4, 4), (5, 5), (6, 6), (7, 7), (8, 8)]

# Initialize the Pygame window
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Sudoku Solver")

def draw_grid():
    for i in range(1, GRID_SIZE):
        if i % 3 == 0:
            pygame.draw.line(screen, BLACK, (i * CELL_SIZE, 0), (i * CELL_SIZE, HEIGHT), 2)
            pygame.draw.line(screen, BLACK, (0, i * CELL_SIZE), (WIDTH, i * CELL_SIZE), 2)
        else:
            pygame.draw.line(screen, GRAY, (i * CELL_SIZE, 0), (i * CELL_SIZE, HEIGHT), 1)
            pygame.draw.line(screen, GRAY, (0, i * CELL_SIZE), (WIDTH, i * CELL_SIZE), 1)

def draw_numbers(sudoku_values):
    font = pygame.font.Font(None, 36)
    for i in range(GRID_SIZE):
        for j in range(GRID_SIZE):
            value = int(sudoku_values[i * GRID_SIZE + j])
            if (i, j) in colored_cells:
                pygame.draw.rect(screen, RED, (j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE), 0)
            if value != 0:
                text = font.render(str(value), True, BLACK)
                text_rect = text.get_rect(center=(j * CELL_SIZE + CELL_SIZE // 2, i * CELL_SIZE + CELL_SIZE // 2))
                screen.blit(text, text_rect)

def main():
    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()

        screen.fill(WHITE)

        draw_numbers(sudoku_input)
        draw_grid()

        pygame.display.flip()

if __name__ == "__main__":
    main()