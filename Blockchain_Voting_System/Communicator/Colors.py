from colorama import init, Fore, Style
from enum import Enum

ColorEnum = Enum("ColorEnum", "RED GREEN YELLOW BLUE MAGENTA CYAN WHITE BLACK")

class Colors():
    RED=Fore.RED
    GREEN=Fore.GREEN
    YELLOW=Fore.YELLOW
    BLUE=Fore.BLUE
    MAGENTA=Fore.MAGENTA
    CYAN=Fore.CYAN
    WHITE=Fore.WHITE
    BLACK=Fore.BLACK
     # Map color names to colorama Fore constants
    color_map = {
        "black": Fore.BLACK,
        "red": Fore.LIGHTRED_EX,
        "green": Fore.GREEN,
        "yellow": Fore.YELLOW,
        "blue": Fore.BLUE,
        "magenta": Fore.MAGENTA,
        "cyan": Fore.CYAN,
        "white": Fore.WHITE,
    }

def print_colored_text(text, color):
    # Initialize colorama
    init()
    # Validate the color input
    color = color.lower()
    if color not in Colors.color_map.keys() and color not in ColorEnum.__members__:
        raise ValueError(f"Invalid color: {color}. Choose from {', '.join(Colors.color_map.keys())}.")

    # Print the colored text
    if color in Colors.color_map.values():
        print(f"{Colors.color_map.values()[color]}{text}{Style.RESET_ALL}")
        return
    print(f"{Colors.color_map[color]}{text}{Style.RESET_ALL}")
    
    
if __name__== "__main__":
    # testing colors
    print_colored_text("This is green text!", "green")
    print_colored_text("This is red text!", "red")
    print_colored_text("This is blue text!", "blue")
    print_colored_text("This is blue text!", ColorEnum.BLUE.name)
    
    print(Fore.BLACK + 'BLACK')
    print(Fore.BLUE + 'BLUE')
    print(Fore.CYAN + 'CYAN')
    print(Fore.GREEN + 'GREEN')
    print(Fore.LIGHTBLACK_EX + 'LIGHTBLACK_EX')
    print(Fore.LIGHTBLUE_EX + 'LIGHTBLUE_EX')
    print(Fore.LIGHTCYAN_EX + 'LIGHTCYAN_EX')
    print(Fore.LIGHTGREEN_EX + 'LIGHTGREEN_EX')
    print(Fore.LIGHTMAGENTA_EX + 'LIGHTMAGENTA_EX')
    print(Fore.LIGHTRED_EX + 'LIGHTRED_EX')
    print(Fore.LIGHTWHITE_EX + 'LIGHTWHITE_EX')
    print(Fore.LIGHTYELLOW_EX + 'LIGHTYELLOW_EX')
    print(Fore.MAGENTA + 'MAGENTA')
    print(Fore.RED + 'RED')
    print(Fore.RESET + 'RESET')
    print(Fore.WHITE + 'WHITE')
    print(Fore.YELLOW + 'YELLOW')