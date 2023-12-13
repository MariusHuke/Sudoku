import pygame
import sys
import Sudoku
import os
import matplotlib.pyplot as plt
from matplotlib.backends.backend_agg import FigureCanvasAgg
#compiled with pyinstaller --onefile --hidden-import=pygame --add-data='Sudoku.cpython-310-x86_64-linux-gnu.so:.' --add-data='testdata.txt:.' GUI.py 
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

def draw_matplotlib_graph(average, best):
    # Create a simple matplotlib graph (replace this with your own graph logic)
    fig, ax = plt.subplots()
    ax.plot(average, label='Durchschnitt')
    ax.plot(best, label='Beste')
    ax.set_title('Fitnessanalyse')
    ax.set_xlabel('Generationen')
    ax.set_ylabel('Fitness')
    ax.legend()

    # Render the matplotlib graph to a Pygame surface
    canvas = FigureCanvasAgg(fig)
    canvas.draw()
    renderer = canvas.get_renderer()
    raw_data = renderer.tostring_rgb()

    # Create a Pygame image from the rendered matplotlib graph
    size = canvas.get_width_height()
    graph_surface = pygame.image.fromstring(raw_data, size, "RGB")

    # Display the Pygame image on the screen
    screen.blit(graph_surface, (WIDTH + 220, 0))

    return fig

def start_screen():
    #init textboxes
    font = pygame.font.Font(None, 36)
    population_input_box = pygame.Rect(400, 200, 140, 32)
    which_input_box = pygame.Rect(400, 250, 140, 32)
    color_inactive = pygame.Color('lightskyblue3')
    color_active = pygame.Color('dodgerblue2')
    population_color = color_inactive
    which_color = color_inactive
    population_active = False
    which_active = False
    population_text = ''
    which_text = ''
    
    #init text
    title_font = pygame.font.Font(None, 28)
    title_text1 = title_font.render("(0-9 leicht, 10-19 mittel,", True, BLACK)
    title_text2 = title_font.render("20-29 schwer, 30-39 extrem)", True, BLACK)

    label_font = pygame.font.Font(None, 36)
    label1 = label_font.render("Populationsgröße:", True, BLACK)
    label2 = label_font.render("Welches Sudoku:", True, BLACK)

    #init button
    ok_button_width, ok_button_height = 100, 40
    ok_button_x, ok_button_y = (WIDTH + 200) // 2 - ok_button_width // 2, 400

    #UpdateLoop
    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            #check if textbox is clicked
            if event.type == pygame.MOUSEBUTTONDOWN:
                mouse_x, mouse_y = pygame.mouse.get_pos()
                if ok_button_x < mouse_x < ok_button_x + ok_button_width and ok_button_y < mouse_y < ok_button_y + ok_button_height:
                    try:
                        population_size = int(population_text)
                        which_value = int(which_text)
                        return population_size, which_value
                    except ValueError:
                        print("Invalid input. Please enter integers.")
                if population_input_box.collidepoint(event.pos):
                    population_active = not population_active
                    which_active = False
                elif which_input_box.collidepoint(event.pos):
                    which_active = not which_active
                    population_active = False
                else:
                    population_active = False
                    which_active = False
                population_color = color_active if population_active else color_inactive
                which_color = color_active if which_active else color_inactive
            #get input (enter for submit, backspace for delete)
            if event.type == pygame.KEYDOWN:
                if population_active or which_active:
                    if event.key == pygame.K_RETURN:
                        try:
                            population_size = int(population_text)
                            which_value = int(which_text)
                            return population_size, which_value
                        except ValueError:
                            print("Invalid input. Please enter integers.")
                        continue
                if population_active:    
                    if event.key == pygame.K_BACKSPACE:
                        population_text = population_text[:-1]
                    else:
                        population_text += event.unicode
                if which_active:    
                    if event.key == pygame.K_BACKSPACE:
                        which_text = which_text[:-1]
                    else:
                        which_text += event.unicode

        #draw texts
        screen.fill(WHITE)
        screen.blit(title_text1, ((WIDTH+200) // 2 - title_text1.get_width() // 2, 300))
        screen.blit(title_text2, ((WIDTH+200) // 2 - title_text2.get_width() // 2, 350))
        screen.blit(label1, (150, 200))
        screen.blit(label2, (150, 250))

        #draw input boxes
        txt_surface1 = font.render(population_text, True, population_color)
        width1 = max(200, txt_surface1.get_width()+10)
        population_input_box.w = width1
        screen.blit(txt_surface1, (population_input_box.x+5, population_input_box.y+5))
        pygame.draw.rect(screen, population_color, population_input_box, 2)

        txt_surface2 = font.render(which_text, True, which_color)
        width2 = max(200, txt_surface2.get_width()+10)
        which_input_box.w = width2
        screen.blit(txt_surface2, (which_input_box.x+5, which_input_box.y+5))
        pygame.draw.rect(screen, which_color, which_input_box, 2)
        
        #draw ok button
        pygame.draw.rect(screen, GRAY, (ok_button_x, ok_button_y, ok_button_width, ok_button_height))
        ok_button_text = font.render("OK", True, BLACK)
        ok_button_text_rect = ok_button_text.get_rect(center=(ok_button_x + ok_button_width // 2, ok_button_y + ok_button_height // 2))
        screen.blit(ok_button_text, ok_button_text_rect)
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

#draw everything
def draw(sudoku_input, colored_cells, originals, generation, collisions, averages, 
         best, step_size_text, step_size_color, selection_size_text, selection_color, 
         step_size_input_box, selection_input_box, step_button_x, step_button_y, step_button_width, step_button_height, step_text,
         step_text_rect, reset_button_x, reset_button_y, reset_button_width, reset_button_height, reset_text, reset_text_rect, 
         population_button_x, population_reset_button_y, population_button_width, population_button_height, population_text, population_text_rect):
        #draw
        screen.fill(WHITE)

        #text
        draw_numbers(sudoku_input,colored_cells, originals)
        draw_grid()
        display_text("Generation: ", WIDTH + 20, 50)
        display_text(str(generation), WIDTH + 20, 75)
        display_text("Kollisionen: ", WIDTH + 20, 150)
        display_text(str(collisions), WIDTH + 20, 175)
        display_text("Generationen ", WIDTH + 20, 225)
        display_text("pro Schritt: ", WIDTH + 20, 250)
        display_text("Selektions- ", WIDTH + 20, 400)
        display_text("rate (1-100%): ", WIDTH + 20, 425)

        #button and input box
        #step
        font = pygame.font.Font(None, 36)
        txt_surface1 = font.render(step_size_text, True, step_size_color)
        width1 = max(100, txt_surface1.get_width()+10)
        step_size_input_box.w = width1
        screen.blit(txt_surface1, (step_size_input_box.x+5, step_size_input_box.y+5))
        pygame.draw.rect(screen, step_size_color, step_size_input_box, 2)
        #selection
        txt_surface2 = font.render(selection_size_text, True, selection_color)
        width2 = max(100, txt_surface2.get_width()+10)
        selection_input_box.w = width2
        screen.blit(txt_surface2, (selection_input_box.x+5, selection_input_box.y+5))
        pygame.draw.rect(screen, selection_color, selection_input_box, 2)
        #buttons
        pygame.draw.rect(screen, GRAY, (step_button_x, step_button_y, step_button_width, step_button_height))
        screen.blit(step_text, step_text_rect)
        pygame.draw.rect(screen, GRAY, (reset_button_x, reset_button_y, reset_button_width, reset_button_height))
        screen.blit(reset_text, reset_text_rect)
        pygame.draw.rect(screen, GRAY, (population_button_x, population_reset_button_y, population_button_width, population_button_height))
        screen.blit(population_text, population_text_rect)
        plt.close('all')
        draw_matplotlib_graph(averages,best)

        pygame.display.flip()
def main(population_size=None, which=None):
    new = 0
    #init generation variables
    if(population_size==None or which==None):
        pygame.display.set_mode((WIDTH+210, HEIGHT))
        population_size, which = start_screen()
    pygame.display.set_mode((WIDTH+850, HEIGHT))
    script_dir = os.path.dirname(os.path.abspath(__file__))
    datapath = os.path.join(script_dir, "testdata.txt")
    # Make the path separator platform-independent
    datapath = os.path.normpath(datapath)
    sudokusolver = Sudoku.Solver(which,population_size,os.path.join(datapath))
    a = sudokusolver.step(20)
    sudoku_input = a[:81]
    colored_cells = a[81:-2]
    averages = [a[-2]]
    best = [a[-1]]
    solved = False
    #read original input
    originals = []
    file = open(datapath,"r")
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
    step_size_input_box = pygame.Rect(WIDTH+20, 300, 140, 32)
    step_size_text = '1'
    step_active = False
    color_inactive = pygame.Color('lightskyblue3')
    color_active = pygame.Color('dodgerblue2')
    step_size_color = color_inactive
    #selection
    selection_color = color_inactive
    selection_input_box = pygame.Rect(WIDTH+20, 475, 140, 32)
    selection_size_text = '20'
    selection_active = False

    #init button parameters
    step_button_width, step_button_height = 200, 50
    step_button_x, step_button_y = WIDTH, 525
    
    #init button parameters
    reset_button_width, reset_button_height = 200, 50
    reset_button_x, reset_button_y = WIDTH+250, 525

    #init button parameters
    population_button_width, population_button_height = 200, 50
    population_button_x, population_reset_button_y = WIDTH+500, 525

    #init text parameters
    font = pygame.font.Font(None, 36)
    step_text = font.render("Schritt", True, BLACK)
    step_text_rect = step_text.get_rect(center=(step_button_x + step_button_width // 2, step_button_y + step_button_height // 2))
    reset_text = font.render("Reset", True, BLACK)
    reset_text_rect = reset_text.get_rect(center=(reset_button_x + reset_button_width // 2, reset_button_y + reset_button_height // 2))
    population_text = font.render("0. Gen", True, BLACK)
    population_text_rect = population_text.get_rect(center=(population_button_x + population_button_width // 2, population_reset_button_y + population_button_height // 2))

    #UpdateLoop
    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            #input box for step size
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_RETURN:
                    step_active = False
                    step_size_color = color_inactive
                    selection_active = False
                    selection_color = color_inactive
                    continue
                if step_active:    
                    if event.key == pygame.K_BACKSPACE:
                        step_size_text = step_size_text[:-1]
                    else:
                        step_size_text += event.unicode
                if selection_active:
                    if event.key == pygame.K_BACKSPACE:
                        selection_size_text = selection_size_text[:-1]
                    else:
                        selection_size_text += event.unicode
            #input box and button for step size
            if event.type == pygame.MOUSEBUTTONDOWN:  
                #input box  
                if step_size_input_box.collidepoint(event.pos):
                    step_active = True
                    selection_active = False
                    step_size_color = color_active
                    selection_color = color_inactive
                elif selection_input_box.collidepoint(event.pos):
                    selection_active = True
                    step_active = False
                    selection_color = color_active
                    step_size_color = color_inactive
                else:
                    step_active = False
                    step_size_color = color_inactive
                    selection_active = False
                    selection_color = color_inactive


                #button
                mouse_x, mouse_y = pygame.mouse.get_pos()
                # Check if the mouse click is within the button boundaries
                if step_button_x < mouse_x < step_button_x + step_button_width and step_button_y < mouse_y < step_button_y + step_button_height and not solved:
                    try:
                        #perform steps (break early if sudoku is already solved)
                        if int(step_size_text) < 1:
                            step_size_text = '1'
                        if int(selection_size_text) < 1 or int(selection_size_text) > 100:
                            selection_size_text = '20'
                        for i in range(int(step_size_text)):
                            a = sudokusolver.step(int(selection_size_text))
                            averages.append(a[-2])
                            best.append(a[-1])
                            generation+=1
                            sudoku_input = a[:81]
                            colored_cells = a[81:-2]
                            collisions = sum(colored_cells)
                            draw(sudoku_input, colored_cells, originals, generation, collisions, averages, best, 
                                 step_size_text, step_size_color, selection_size_text, selection_color, step_size_input_box, 
                                 selection_input_box, step_button_x, step_button_y, step_button_width, step_button_height, step_text,
                                 step_text_rect, reset_button_x, reset_button_y, reset_button_width, reset_button_height, reset_text, reset_text_rect, 
                                 population_button_x, population_reset_button_y, population_button_width, population_button_height, population_text, population_text_rect)
                            if sum(a[81:-2]) == 0:
                                solved = True
                                break
                    except ValueError:
                        print("Invalid input. Please enter integers.")
                elif reset_button_x < mouse_x < reset_button_x + reset_button_width and reset_button_y < mouse_y < reset_button_y + reset_button_height:
                    new = 1
                    break
                elif population_button_x < mouse_x < population_button_x + population_button_width and population_reset_button_y < mouse_y < population_reset_button_y + population_button_height:
                    new = 2
                    break
        draw(sudoku_input, colored_cells, originals, generation, collisions, averages, best, 
                                    step_size_text, step_size_color, selection_size_text, selection_color, step_size_input_box, 
                                    selection_input_box, step_button_x, step_button_y, step_button_width, step_button_height, step_text,
                                    step_text_rect, reset_button_x, reset_button_y, reset_button_width, reset_button_height, reset_text, reset_text_rect,
                                    population_button_x, population_reset_button_y, population_button_width, population_button_height, population_text, population_text_rect)
        if new!=0:
            break
    if new==1:
        main()
    elif new==2:
        main(population_size, which)
if __name__ == "__main__":
    main()