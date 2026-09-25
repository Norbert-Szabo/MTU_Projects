class Lego:

    def __init__(self, set_number, title, num_pieces, retail_price, in_stock):
        self.set_number = set_number
        self.title = title
        self.num_pieces = num_pieces
        self.retail_price = retail_price
        self.in_stock = bool(in_stock)

    def __str__(self):
        if self.in_stock:
            return f'The {self.title} (set number {self.set_number}) has {self.num_pieces} pieces, has a retail price of €{self.retail_price} and it is in stock.'
        else:
            return f'The {self.title} (set number {self.set_number}) has {self.num_pieces} pieces, has a retail price of €{self.retail_price} and it is not in stock.'
