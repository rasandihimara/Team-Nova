#pragma once
#include <string>
#include <vector>

// Product demonstrates encapsulation: data is private and changed through methods.
class Product
{
private:
    std::string id;
    std::string name;
    std::string category;
    double price;
    int stock;
    int reorderLevel;

public:
    Product(std::string id, std::string name, std::string category,
            double price, int stock, int reorderLevel);

    std::string getId() const;
    std::string getName() const;
    std::string getCategory() const;
    double getPrice() const;
    int getStock() const;
    int getReorderLevel() const;

    void setName(std::string value);
    void setCategory(std::string value);
    void setPrice(double value);
    void setStock(int value);
    void setReorderLevel(int value);
    bool isLowStock() const;
};

// Abstraction: User is an abstract base class.
class User
{
protected:
    std::string username;
    std::string password;

public:
    User(std::string username, std::string password);
    virtual ~User() = default;
    bool checkPassword(const std::string& value) const;
    virtual std::string getRole() const = 0;
};

// Inheritance: Cashier and Manager inherit from User.
class Cashier : public User
{
public:
    Cashier(std::string username, std::string password);
    std::string getRole() const override;
};

class Manager : public User
{
public:
    Manager(std::string username, std::string password);
    std::string getRole() const override;
};

// Simple order line used by SalesForm.
struct SaleLine
{
    std::string productId;
    std::string productName;
    int quantity;
    double unitPrice;

    double subtotal() const { return quantity * unitPrice; }
};

// Small in-memory manager. No database is used for this version.
class BakerySystem
{
private:
    std::vector<Product> products;
    std::vector<SaleLine> currentSale;
    int orderNumber;
    double totalSales;

public:
    BakerySystem();

    std::vector<Product>& getProducts();
    const std::vector<Product>& getProducts() const;
    std::vector<SaleLine>& getCurrentSale();
    const std::vector<SaleLine>& getCurrentSale() const;

    Product* findProduct(const std::string& id);
    void clearProducts();
    bool addProduct(Product product);
    bool updateProduct(const std::string& id, std::string name, std::string category,
                       double price, int stock, int reorderLevel);
    bool deleteProduct(const std::string& id);

    bool addSaleLine(const std::string& id, int quantity);
    bool removeSaleLine(int index);
    void clearSale();
    double getSaleTotal() const;
    double getTotalSales() const;
    int getOrderNumber() const;
    bool completeSale();
    int getLowStockCount() const;
};
