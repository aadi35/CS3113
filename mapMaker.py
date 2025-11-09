import pygame
import sys

ROWS = 6   # change grid size here
COLS = 20
CELL_SIZE = 60

pygame.init()
screen = pygame.display.set_mode((COLS * CELL_SIZE, ROWS * CELL_SIZE))
pygame.display.set_caption("Grid Painter - Press number keys to change paint value, E to export")

font = pygame.font.SysFont(None, 32)

# Grid data (default = 0)
grid = [[0 for _ in range(COLS)] for _ in range(ROWS)]

paint_value = 1  # default paint value

def draw_grid():
    screen.fill((255, 255, 255))
    for r in range(ROWS):
        for c in range(COLS):
            rect = pygame.Rect(c * CELL_SIZE, r * CELL_SIZE, CELL_SIZE, CELL_SIZE)
            pygame.draw.rect(screen, (0, 0, 0), rect, 1)

            value = grid[r][c]
            if value != 0:
                text = font.render(str(value), True, (0, 0, 255))
                screen.blit(text, (c * CELL_SIZE + CELL_SIZE//3, r * CELL_SIZE + CELL_SIZE//4))

def export_array():
    print("{")
    for r in range(ROWS):
        row_str = ", ".join(str(grid[r][c]) for c in range(COLS))
        print(f"    {row_str},")
    print("};\n")


while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            sys.exit()

        # Paint when clicking
        if event.type == pygame.MOUSEBUTTONDOWN:
            x, y = event.pos
            c, r = x // CELL_SIZE, y // CELL_SIZE
            if 0 <= r < ROWS and 0 <= c < COLS:
                grid[r][c] = paint_value

        # Change paint value with number keys
        if event.type == pygame.KEYDOWN:
            if pygame.K_0 <= event.key <= pygame.K_9:
                paint_value = event.key - pygame.K_0
                print(f"Paint value set to {paint_value}")

            # Press E to export
            if event.key == pygame.K_e:
                export_array()

    draw_grid()
    pygame.display.flip()
