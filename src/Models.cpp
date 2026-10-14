#include "../include/Models.h"

Product::Product(std::string id, std::string name, std::string category,
                 double price, int stock, int reorderLevel)
    : id(id), name(name), category(category), price(price), stock(stock), reorderLevel(reorderLevel) {}

std::string Product::getId() const { return id; }
std::string Product::getName() const { return name; }
std::string Product::getCategory() const { return category; }
double Product::getPrice() const { return price; }
int Product::getStock() const { return stock; }
int Product::getReorderLevel() const { return reorderLevel; }
void Product::setName(std::string value) { name = value; }
void Product::setCategory(std::string value) { category = value; }
void Product::setPrice(double value) { price = value; }
void Product::setStock(int value) { stock = value; }
void Product::setReorderLevel(int value) { reorderLevel = value; }
bool Product::isLowStock() const { return stock <= reorderLevel; }

User::User(std::string username, std::string password) : username(username), password(password) {}
bool User::checkPassword(const std::string& value) const { return password == value; }
Cashier::Cashier(std::string username, std::string password) : User(username, password) {}
Manager::Manager(std::string username, std::string password) : User(username, password) {}
std::string Cashier::getRole() const { return "Cashier"; }
std::string Manager::getRole() const { return "Manager"; }

BakerySystem::BakerySystem() : orderNumber(6), totalSales(4370.00)
{
    products.emplace_back("I1", "Butter Cake 500g", "Cakes", 850.00, 15, 5);
    products.emplace_back("I2", "Chocolate Gateau", "Cakes", 2200.00, 8, 10);
    products.emplace_back("I3", "Chicken Bun", "Savory", 120.00, 60, 20);
    products.emplace_back("I4", "Fish Roll", "Savory", 100.00, 50, 15);
    products.emplace_back("I5", "Garlic Bread", "Bread", 350.00, 25, 10);
    products.emplace_back("I6", "Chocolate Chip Cookie", "Cookies", 150.00, 80, 15);
}

std::vector<Product>& BakerySystem::getProducts() { return products; }
const std::vector<Product>& BakerySystem::getProducts() const { return products; }
std::vector<SaleLine>& BakerySystem::getCurrentSale() { return currentSale; }
const std::vector<SaleLine>& BakerySystem::getCurrentSale() const { return currentSale; }

Product* BakerySystem::findProduct(const std::string& id)
{
    for (auto& product : products)
        if (product.getId() == id) return &product;
    return nullptr;
}

void BakerySystem::clearProducts() { products.clear(); }

bool BakerySystem::addProduct(Product product)
{
    if (findProduct(product.getId()) != nullptr) return false;
    products.push_back(product);
    return true;
}

bool BakerySystem::updateProduct(const std::string& id, std::string name, std::string category,
                                 double price, int stock, int reorderLevel)
{
    Product* product = findProduct(id);
    if (!product) return false;
    product->setName(name);
    product->setCategory(category);
    product->setPrice(price);
    product->setStock(stock);
    product->setReorderLevel(reorderLevel);
    return true;
}

bool BakerySystem::deleteProduct(const std::string& id)
{
    for (auto it = products.begin(); it != products.end(); ++it)
    {
        if (it->getId() == id) { products.erase(it); return true; }
    }
    return false;
}

bool BakerySystem::addSaleLine(const std::string& id, int quantity)
{
    Product* product = findProduct(id);
    if (!product || quantity <= 0 || quantity > product->getStock()) return false;

    for (auto& line : currentSale)
    {
        if (line.productId == id)
        {
            if (line.quantity + quantity > product->getStock()) return false;
            line.quantity += quantity;
            return true;
        }
    }

    currentSale.push_back({ id, product->getName(), quantity, product->getPrice() });
    return true;
}

bool BakerySystem::removeSaleLine(int index)
{
    if (index < 0 || index >= static_cast<int>(currentSale.size())) return false;
    currentSale.erase(currentSale.begin() + index);
    return true;
}

void BakerySystem::clearSale() { currentSale.clear(); }

double BakerySystem::getSaleTotal() const
{
    double total = 0;
    for (const auto& line : currentSale) total += line.subtotal();
    return total;
}

double BakerySystem::getTotalSales() const { return totalSales; }
int BakerySystem::getOrderNumber() const { return orderNumber; }

bool BakerySystem::completeSale()
{
    if (currentSale.empty()) return false;
    for (const auto& line : currentSale)
    {
        Product* product = findProduct(line.productId);
        if (!product || line.quantity > product->getStock()) return false;
    }
    for (const auto& line : currentSale)
        findProduct(line.productId)->setStock(findProduct(line.productId)->getStock() - line.quantity);

    totalSales += getSaleTotal();
    orderNumber++;
    clearSale();
    return true;
}

int BakerySystem::getLowStockCount() const
{
    int count = 0;
    for (const auto& p : products) if (p.isLowStock()) count++;
    return count;
}
