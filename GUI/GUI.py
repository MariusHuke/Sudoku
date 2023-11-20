import pygame
import sys
import Sudoku

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
GREEN = (0, 255, 0)

# Initialize the Pygame window
screen = pygame.display.set_mode((WIDTH+210, HEIGHT))
pygame.display.set_caption("Sudoku Solver")

#2 input boxes for population_size and which sudoku to solve
def start_screen():
    #init textboxes
    font = pygame.font.Font(None, 36)
    input_box1 = pygame.Rect(400, 200, 140, 32)
    input_box2 = pygame.Rect(400, 250, 140, 32)
    color_inactive = pygame.Color('lightskyblue3')
    color_active = pygame.Color('dodgerblue2')
    color1 = color_inactive
    color2 = color_inactive
    active1 = False
    active2 = False
    text1 = ''
    text2 = ''
    
    #init text
    title_font = pygame.font.Font(None, 28)
    title_text1 = title_font.render("(0-9 leicht, 10-19 mittel,", True, BLACK)
    title_text2 = title_font.render("20-29 schwer, 30-39 extrem)", True, BLACK)

    label_font = pygame.font.Font(None, 36)
    label1 = label_font.render("Populationsgröße:", True, BLACK)
    label2 = label_font.render("Welches Sudoku:", True, BLACK)

    #UpdateLoop
    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            #check if textbox is clicked
            if event.type == pygame.MOUSEBUTTONDOWN:
                if input_box1.collidepoint(event.pos):
                    active1 = not active1
                    active2 = False
                elif input_box2.collidepoint(event.pos):
                    active2 = not active2
                    active1 = False
                else:
                    active1 = False
                    active2 = False
                color1 = color_active if active1 else color_inactive
                color2 = color_active if active2 else color_inactive
            #get input (enter for submit, backspace for delete)
            if event.type == pygame.KEYDOWN:
                if active1 or active2:
                    if event.key == pygame.K_RETURN:
                        try:
                            population_size = int(text1)
                            which_value = int(text2)
                            while(population_size % 3 !=  0):
                                population_size += 1
                            return population_size, which_value
                        except ValueError:
                            print("Invalid input. Please enter integers.")
                if active1:    
                    if event.key == pygame.K_BACKSPACE:
                        text1 = text1[:-1]
                    else:
                        text1 += event.unicode
                if active2:    
                    if event.key == pygame.K_BACKSPACE:
                        text2 = text2[:-1]
                    else:
                        text2 += event.unicode

        #draw texts
        screen.fill(WHITE)
        screen.blit(title_text1, ((WIDTH+200) // 2 - title_text1.get_width() // 2, 300))
        screen.blit(title_text2, ((WIDTH+200) // 2 - title_text2.get_width() // 2, 350))
        screen.blit(label1, (150, 200))
        screen.blit(label2, (150, 250))

        #draw input boxes
        txt_surface1 = font.render(text1, True, color1)
        width1 = max(200, txt_surface1.get_width()+10)
        input_box1.w = width1
        screen.blit(txt_surface1, (input_box1.x+5, input_box1.y+5))
        pygame.draw.rect(screen, color1, input_box1, 2)

        txt_surface2 = font.render(text2, True, color2)
        width2 = max(200, txt_surface2.get_width()+10)
        input_box2.w = width2
        screen.blit(txt_surface2, (input_box2.x+5, input_box2.y+5))
        pygame.draw.rect(screen, color2, input_box2, 2)

        pygame.display.flip()

#draw outlines of the grid
def draw_grid():
    for i in range(1, GRID_SIZE):
        if i % 3 == 0:
            pygame.draw.line(screen, BLACK, (i * CELL_SIZE, 0), (i * CELL_SIZE, HEIGHT), 2)
            pygame.draw.line(screen, BLACK, (0, i * CELL_SIZE), (WIDTH, i * CELL_SIZE), 2)
        else:
            pygame.draw.line(screen, GRAY, (i * CELL_SIZE, 0), (i * CELL_SIZE, HEIGHT), 1)
            pygame.draw.line(screen, GRAY, (0, i * CELL_SIZE), (WIDTH, i * CELL_SIZE), 1)

#helper to draw text
def display_text(text, x, y, font_size=40, color=BLACK):
    font = pygame.font.Font(None, font_size)
    text_surface = font.render(text, True, color)
    screen.blit(text_surface, (x, y))

#draw numbers in the grid, paint the cells red if they are in conflict, green if they are original
def draw_numbers(sudoku_values, color_values, originals):
    font = pygame.font.Font(None, 36)
    for i in range(GRID_SIZE):
        for j in range(GRID_SIZE):
            value = sudoku_values[i * GRID_SIZE + j]
            if color_values[i * GRID_SIZE + j] != 0:
                pygame.draw.rect(screen, RED, (j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE), 0)
            if originals[i * GRID_SIZE + j] != 0:
                pygame.draw.rect(screen, GREEN, (j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE), 0)
            if value != 0:
                text = font.render(str(value), True, BLACK)
                text_rect = text.get_rect(center=(j * CELL_SIZE + CELL_SIZE // 2, i * CELL_SIZE + CELL_SIZE // 2))
                screen.blit(text, text_rect)

def main():
    #init generation variables
    population_size, which = start_screen()
    sudokusolver = Sudoku.Solver(which,population_size)
    a = sudokusolver.step()
    sudoku_input = a[:81]
    colored_cells = a[81:]
    solved = False
    #read original input
    originals = []
    file = open("../data/testdata","r")
    i = -1
    generation = 0
    collisions = sum(colored_cells)
    for line in file:
        if line[0]=="#": #ignore comments
            continue
        else:
            i+=1
        if i == which:
            originals = [int(char) for char in line[:-1]]

    #init input box for step size
    input_box = pygame.Rect(WIDTH+20, 425, 140, 32)
    input_text = '1'
    active = False
    color_inactive = pygame.Color('lightskyblue3')
    color_active = pygame.Color('dodgerblue2')

    #init button parameters
    button_width, button_height = 200, 50
    button_x, button_y = WIDTH, 500
    color = color_inactive

    #init text parameters
    font = pygame.font.Font(None, 36)
    text = font.render("Schritt", True, BLACK)
    text_rect = text.get_rect(center=(button_x + button_width // 2, button_y + button_height // 2))

    #UpdateLoop
    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            #input box for step size
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_RETURN:
                    active = not active
                    color = color_active if active else color_inactive
                    continue
                if active:    
                    if event.key == pygame.K_BACKSPACE:
                        input_text = input_text[:-1]
                    else:
                        input_text += event.unicode
            #input box and button for step size
            if event.type == pygame.MOUSEBUTTONDOWN:  
                #input box  
                if input_box.collidepoint(event.pos):
                    active = not active
                    color = color_active if active else color_inactive

                #button
                mouse_x, mouse_y = pygame.mouse.get_pos()
                # Check if the mouse click is within the button boundaries
                if button_x < mouse_x < button_x + button_width and button_y < mouse_y < button_y + button_height and not solved:
                    try:
                        #perform steps (break early if sudoku is already solved)
                        for i in range(int(input_text)):
                            a = sudokusolver.step()
                            if sum(a[81:]) == 0:
                                solved = True
                                break
                            generation+=1
                        sudoku_input = a[:81]
                        colored_cells = a[81:]
                        collisions = sum(colored_cells)
                    except ValueError:
                        print("Invalid input. Please enter integers.")

        #draw
        screen.fill(WHITE)

        #text
        draw_numbers(sudoku_input,colored_cells, originals)
        draw_grid()
        display_text("Generation: ", WIDTH + 20, 50)
        display_text(str(generation), WIDTH + 20, 100)
        display_text("Kollisionen: ", WIDTH + 20, 200)
        display_text(str(collisions), WIDTH + 20, 250)
        display_text("Generationen ", WIDTH + 20, 350)
        display_text("pro Schritt: ", WIDTH + 20, 375)

        #button and input box
        font = pygame.font.Font(None, 36)
        txt_surface1 = font.render(input_text, True, color)
        width1 = max(100, txt_surface1.get_width()+10)
        input_box.w = width1
        screen.blit(txt_surface1, (input_box.x+5, input_box.y+5))
        pygame.draw.rect(screen, color, input_box, 2)
        pygame.draw.rect(screen, GRAY, (button_x, button_y, button_width, button_height))
        screen.blit(text, text_rect)

        pygame.display.flip()

if __name__ == "__main__":
    main()