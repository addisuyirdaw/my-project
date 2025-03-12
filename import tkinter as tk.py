import tkinter as tk
from tkinter import messagebox

class Product:
    def __init__(self, name, price, quantity):
        self.name = name
        self.price = price
        self.quantity = quantity

class MiniMarketApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Mini Market Management System")

        # Initialize product list
        self.products = []

        # Create UI components
        self.create_widgets()

    def create_widgets(self):
        # Labels and Entries for Product Name, Price, and Quantity
        tk.Label(self.root, text="Product Name:").grid(row=0, column=0, padx=10, pady=10)
        self.name_entry = tk.Entry(self.root)
        self.name_entry.grid(row=0, column=1, padx=10, pady=10)

        tk.Label(self.root, text="Price:").grid(row=1, column=0, padx=10, pady=10)
        self.price_entry = tk.Entry(self.root)
        self.price_entry.grid(row=1, column=1, padx=10, pady=10)

        tk.Label(self.root, text="Quantity:").grid(row=2, column=0, padx=10, pady=10)
        self.quantity_entry = tk.Entry(self.root)
        self.quantity_entry.grid(row=2, column=1, padx=10, pady=10)

        # Add Product Button
        self.add_button = tk.Button(self.root, text="Add Product", command=self.add_product)
        self.add_button.grid(row=3, column=0, columnspan=2, pady=10)

        # Listbox to display products
        self.product_listbox = tk.Listbox(self.root, width=50)
        self.product_listbox.grid(row=4, column=0, columnspan=2, padx=10, pady=10)

        # Button to remove selected product
        self.remove_button = tk.Button(self.root, text="Remove Product", command=self.remove_product)
        self.remove_button.grid(row=5, column=0, columnspan=2, pady=10)

    def add_product(self):
        name = self.name_entry.get()
        try:
            price = float(self.price_entry.get())
            quantity = int(self.quantity_entry.get())
        except ValueError:
            messagebox.showerror("Input Error", "Price must be a number and quantity must be an integer")
            return

        new_product = Product(name, price, quantity)
        self.products.append(new_product)
        self.update_product_listbox()

        # Clear entry fields
        self.name_entry.delete(0, tk.END)
        self.price_entry.delete(0, tk.END)
        self.quantity_entry.delete(0, tk.END)

    def update_product_listbox(self):
        self.product_listbox.delete(0, tk.END)
        for product in self.products:
            self.product_listbox.insert(tk.END, f"{product.name} - Price: ${product.price:.2f}, Quantity: {product.quantity}")

    def remove_product(self):
        selected_index = self.product_listbox.curselection()
        if not selected_index:
            messagebox.showwarning("Selection Error", "No product selected")
            return

        del self.products[selected_index[0]]
        self.update_product_listbox()

if __name__ == "__main__":
    root = tk.Tk()
    app = MiniMarketApp(root)
    root.mainloop()
