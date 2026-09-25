import sys
import re


def read_lego_code(prompt: str) -> str:
    """
    Prompts the user to enter a LEGO set code consisting of 5 to 7 digits.

    :param prompt: The message displayed to the user.
    :return: A valid LEGO set code.

    **Example Usage:**
    ```python
    lego_code = read_lego_code("Enter LEGO set code: ")
    print(lego_code)  # Output: "75309"
    ```
    """
    pattern = re.compile(r"^\d{5,7}$")  # Precompile regex for efficiency

    while True:
        s = input(prompt).strip()
        if pattern.fullmatch(s):
            return s
        print("Invalid input. Please enter a number with 5 to 7 digits.")


def read_valid_lego_name(prompt: str) -> str:
    """
    Prompts the user to enter a valid LEGO set name, allowing letters, numbers, spaces, apostrophes, and hyphens.

    :param prompt: The message displayed to the user.
    :return: A valid LEGO set name.

    **Example Usage:**
    ```python
    lego_name = read_valid_lego_name("Enter LEGO set name: ")
    print(lego_name)  # Output: "Star Wars 501st Battle Pack"
    ```
    """
    pattern = re.compile(r"^[A-Za-z0-9' -]+$")  # Now allows digits (0-9)

    while True:
        x = input(prompt).strip()
        if pattern.fullmatch(x):
            return x
        print("Invalid input. LEGO set names can contain letters, numbers, spaces, apostrophes, and hyphens.")


def read_valid_option(prompt: str, options: list[str] | str) -> str:
    """
    Prompts the user to enter a valid option from a predefined list.

    :param prompt: The message displayed to the user.
    :param options: A list of valid options.
    :return: The selected valid option.

    **Example Usage:**
    ```python
    category = read_valid_option("Enter category (Technic/City/Star Wars): ", ["Technic", "City", "Star Wars"])
    print(category)  # Output: "City"
    ```
    """
    options_set = set(options)  # Convert to set for O(1) lookup time

    while True:
        x = input(prompt).strip()
        if x in options_set:
            return x
        print(f"Invalid input. Please choose from {', '.join(options)}.")


def read_integer(prompt: str, lowest: int = -sys.maxsize, highest: int = sys.maxsize) -> int:
    """
    Prompts the user to enter an integer within a specified range.

    :param prompt: The message displayed to the user.
    :param lowest: The minimum acceptable integer value.
    :param highest: The maximum acceptable integer value.
    :return: A valid integer.

    **Example Usage:**
    ```python
    num_pieces = read_integer("Enter the number of pieces: ", 1, 5000)
    print(num_pieces)  # Output: 1000
    ```
    """
    while True:
        try:
            x = int(input(prompt).strip())
            if lowest <= x <= highest:
                return x
            print(f"Please enter a number between {lowest} and {highest}.")
        except ValueError:
            print("Invalid input. Please enter a whole number.")


def read_float(prompt: str, lowest: float = -sys.float_info.max, highest: float = sys.float_info.max) -> float:
    """
    Prompts the user to enter a floating-point number within a specified range.

    :param prompt: The message displayed to the user.
    :param lowest: The minimum acceptable floating-point value.
    :param highest: The maximum acceptable floating-point value.
    :return: A valid floating-point number.

    **Example Usage:**
    ```python
    price = read_float("Enter the price of the LEGO set: ", 0.0, 500.0)
    print(price)  # Output: 299.99
    ```
    """
    while True:
        try:
            x = float(input(prompt).strip())
            if lowest <= x <= highest:
                return x
            print(f"Please enter a number between {lowest} and {highest}.")
        except ValueError:
            print("Invalid input. Please enter a valid number.")
