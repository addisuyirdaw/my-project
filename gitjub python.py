class Product:
    def __init__(self, product_id, name, price, stock):
        self.product_id = product_id
        self.name = name
        self.price = price
        self.stock = stock

class MiniMarket:
    def __init__(self):
        self.products = {}
        self.sales = []

    def add_product(self, product_id, name, price, stock):
        if product_id in self.products:
            print("Product ID already exists.")
            return
        self.products[product_id] = Product(product_id, name, price, stock)
        print(f"Product {name} added successfully.")

    def update_product(self, product_id, name=None, price=None, stock=None):
        if product_id not in self.products:
            print("Product not found.")
            return
        product = self.products[product_id]
        if name:
            product.name = name
        if price:
            product.price = price
        if stock:
            product.stock = stock
        print(f"Product {product_id} updated successfully.")

    def delete_product(self, product_id):
        if product_id not in self.products:
            print("Product not found.")
            return
        del self.products[product_id]
        print(f"Product {product_id} deleted successfully.")

    def check_stock(self, product_id):
        if product_id not in self.products:
            print("Product not found.")
            return
        product = self.products[product_id]
        print(f"Stock for {product.name}: {product.stock}")

    def update_stock(self, product_id, quantity):
        if product_id not in self.products:
            print("Product not found.")
            return
        self.products[product_id].stock += quantity
        print(f"Stock for {self.products[product_id].name} updated successfully. New stock: {self.products[product_id].stock}")

    def process_sale(self, product_id, quantity):
        if product_id not in self.products:
            print("Product not found.")
            return
        product = self.products[product_id]
        if product.stock < quantity:
            print("Insufficient stock.")
            return
        total_price = product.price * quantity
        product.stock -= quantity
        sale = {'product_id': product_id, 'quantity': quantity, 'total_price': total_price}
        self.sales.append(sale)
        print(f"Sale processed: {quantity} x {product.name} = {total_price}")

    def generate_inventory_report(self):
        print("Inventory Report:")
        for product in self.products.values():
            print(f"{product.product_id}: {product.name} - {product.stock} in stock")

    def generate_sales_report(self):
        print("Sales Report:")
        for sale in self.sales:
            product = self.products[sale['product_id']]
            print(f"{sale['product_id']}: {product.name} - {sale['quantity']} sold, Total: {sale['total_price']}")

def main():
    market = MiniMarket()
    while True:
        print("\n1. Add Product")
        print("2. Update Product")
        print("3. Delete Product")
        print("4. Check Stock")
        print("5. Update Stock")
        print("6. Process Sale")
        print("7. Generate Inventory Report")
        print("8. Generate Sales Report")
        print("9. Exit")
        choice = input("Enter your choice: ")
        
        if choice == '1':
            product_id = input("Enter product ID: ")
            name = input("Enter product name: ")
            price = float(input("Enter product price: "))
            stock = int(input("Enter product stock: "))
            market.add_product(product_id, name, price, stock)
        elif choice == '2':
            product_id = input("Enter product ID: ")
            name = input("Enter new name (leave blank to keep current): ")
            price = input("Enter new price (leave blank to keep current): ")
            stock = input("Enter new stock (leave blank to keep current): ")
            market.update_product(product_id, name or None, float(price) if price else None, int(stock) if stock else None)
        elif choice == '3':
            product_id = input("Enter product ID: ")
            market.delete_product(product_id)
        elif choice == '4':
            product_id = input("Enter product ID: ")
            market.check_stock(product_id)
        elif choice == '5':
            product_id = input("Enter product ID: ")
            quantity = int(input("Enter quantity to update: "))
            market.update_stock(product_id, quantity)
        elif choice == '6':
            product_id = input("Enter product ID: ")
            quantity = int(input("Enter quantity: "))
            market.process_sale(product_id, quantity)
        elif choice == '7':
            market.generate_inventory_report()
        elif choice == '8':
            market.generate_sales_report()
        elif choice == '9':
            break
        else:
            print("Invalid choice. Please try again.")

if __name__ == "__main__":
    main()

