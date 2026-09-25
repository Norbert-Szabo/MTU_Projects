from lego_class import Lego
import validation

# Expects CSV format: set_num,title,num_pieces,retail_price,stock
def get_data(filename:str):
    lego_list = []
    with open(filename) as con:
        for line in con:
            set_num, title, num_pieces, retail_price, stock = line.strip().split(',')
            l = Lego(set_num, title, int(num_pieces), float(retail_price), int(stock))
            lego_list.append(l)
    return lego_list

# '#' flags sets with no retail price set
def display_inventory(lego_sets: list):
    print('Lego Sets')
    print('-' * 78)
    for lego_set in lego_sets:
        if lego_set.in_stock:
            is_in_stock = '\u2705'
        else:
            is_in_stock = '\u274C'
        if lego_set.retail_price > 0:
            marker = ''
        else:
            marker = '#'
        print(f'{lego_set.set_number:<8}{lego_set.title:50}{lego_set.num_pieces:8,}{is_in_stock:^5}€{lego_set.retail_price:>7.2f}{marker}')
    print()


def add_new_lego_set(lego_sets: list):
    lego_codes_in_list = []
    for line in lego_sets:
        lego_codes_in_list.append(line.set_number)
    set_number = validation.read_lego_code('What is the set number? ')
    if set_number not in lego_codes_in_list:
        set_name = validation.read_valid_lego_name('What is the set name? ')
        num_pieces = validation.read_integer('How many pieces in the set? ',1)
        retail_price = validation.read_float('What is the retail price? ',0,10000)
        in_stock = validation.read_valid_option('Is it in stock? ',['Yes','No'])
        if in_stock in ('yes','Yes'):
            in_stock = 1
        else:
            in_stock = 0
        l = Lego(set_number, set_name, int(num_pieces), float(retail_price), int(in_stock))
        lego_sets.append(l)
        print()
    else:
        print('Error, Lego set already exists')
        print()

def set_finder(lego_sets: list, pattern: str):
    print()
    count = 0
    for line in lego_sets:
        if line.set_number.startswith(pattern):  #
            print(f'{line.set_number:<8}{line.title:50}')
            count += 1

    if count == 0:
        print(f'No matching codes with prefix {pattern}.'), print()

    print()

def delete_set(lego_sets: list, set_number: str):
    index = -1
    for lego_set in lego_sets:
        if lego_set.set_number == set_number:
            index = lego_sets.index(lego_set)
            break

    if index >= 0:
        print(f'Deleting Lego set: {lego_sets[index].title}')
        print()
        lego_sets.pop(index)
        return lego_sets
    else:
        print('Lego set does not exist.')
        print()


def filter_by_size_range(lego_sets: list, lowest: int, highest: int):
    for lego_set in lego_sets:
        if lowest <= lego_set.num_pieces <= highest:
            if lego_set.in_stock:
                is_in_stock = '\u2705'
            else:
                is_in_stock = '\u274C'
            print(f'{lego_set.set_number:<8}{lego_set.title:50}{is_in_stock:^5}')
    print()

def search_set_by_keyword(lego_sets: list, keyword):
    counter = 0
    for lego_set in lego_sets:
        if keyword in lego_set.title.lower():
            print(f'{lego_set.set_number:<8}{lego_set.title:50}')
            counter = 1
    if counter == 0:
        print(f'No sets find containing the keyword: {keyword}')
    print()

# menu loop
def main():
    lego_sets = get_data('lego_sets.txt')

    while True:

        choice = int(input('1. Display Inventory\n'
                           '2. Add New LEGO® Set\n'
                           '3. Search by Code Prefix\n'
                           '4. Delete a Set\n'
                           '5. Filter by Size Range\n'
                           '6. Search by Keyword\n'
                           '7. Quit\n'
                           'Enter your choice: '))

        if choice == 1:
            display_inventory(lego_sets)

        elif choice == 2:
            add_new_lego_set(lego_sets)

        elif choice == 3:
            code_prefix = input('Enter the first 2 numbers in the set code: ')
            set_finder(lego_sets, code_prefix)


        elif choice == 4:

            set_num = validation.read_lego_code('What is the set number of the set you want to delete? ')

            delete_set(lego_sets, set_num)


        elif choice == 5:

            lowest = validation.read_integer('Enter the least  amount of lego piece in the set: ', 1)

            highest = validation.read_integer('Enter the most  amount of lego piece in the set: ', lowest)

            print()

            filter_by_size_range(lego_sets, lowest, highest)


        elif choice == 6:

            keyword = input('What is the set name your are looking for? ')

            search_set_by_keyword(lego_sets, keyword.lower())

        elif choice == 7:
            with open('lego_sets.txt','w') as con:
                for line in lego_sets:
                    if line.in_stock:
                        in_stock = 1
                    else:
                        in_stock = 0
                    print(f'{line.set_number},{line.title},{line.num_pieces},{line.retail_price},{in_stock}', file=con)
            break

        else:
            print('Invalid choice, try again')

if __name__ == '__main__':
    main()
