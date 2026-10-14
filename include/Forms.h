#pragma once
#include <msclr/marshal_cppstd.h>
#include "Models.h"

using namespace System;
using namespace System::Drawing;
using namespace System::Windows::Forms;

ref class LoginForm;
ref class DashboardForm;
ref class SalesForm;
ref class InventoryForm;

// Base GUI class. Other screens inherit common layout helpers from it.
public ref class AppForm : public Form
{
protected:
    BakerySystem* system;
    String^ role;

    AppForm(BakerySystem* system, String^ role);
    Button^ MakeButton(String^ text, int x, int y, int w, int h);
    Label^ MakeLabel(String^ text, int x, int y, int w, int h, int size);
    void StyleButton(Button^ button);
};

public ref class LoginForm : public Form
{
private:
    BakerySystem* system;
    TextBox^ usernameBox;
    TextBox^ passwordBox;
    ComboBox^ roleBox;
    Label^ messageLabel;

public:
    LoginForm(BakerySystem* system);

private:
    void LoginClicked(Object^ sender, EventArgs^ e);
    void DashboardClosed(Object^ sender, FormClosedEventArgs^ e);
};

public ref class DashboardForm : public AppForm
{
private:
    Label^ itemValue;
    Label^ lowValue;
    Label^ salesValue;
    Label^ orderValue;

public:
    DashboardForm(BakerySystem* system, String^ role);

private:
    void OpenSales(Object^ sender, EventArgs^ e);
    void OpenInventory(Object^ sender, EventArgs^ e);
    void RefreshStats();
    void RefreshClicked(Object^ sender, EventArgs^ e);
    void ExitClicked(Object^ sender, EventArgs^ e);
};

public ref class SalesForm : public AppForm
{
private:
    ComboBox^ productBox;
    NumericUpDown^ quantityBox;
    DataGridView^ saleGrid;
    Label^ totalLabel;

public:
    SalesForm(BakerySystem* system, String^ role);

private:
    void AddItem(Object^ sender, EventArgs^ e);
    void RemoveItem(Object^ sender, EventArgs^ e);
    void CompleteSale(Object^ sender, EventArgs^ e);
    void RefreshProducts();
    void RefreshSaleGrid();
    void CloseClicked(Object^ sender, EventArgs^ e);
};

public ref class InventoryForm : public AppForm
{
private:
    DataGridView^ inventoryGrid;
    TextBox^ idBox;
    TextBox^ nameBox;
    TextBox^ categoryBox;
    NumericUpDown^ priceBox;
    NumericUpDown^ stockBox;
    NumericUpDown^ reorderBox;

public:
    InventoryForm(BakerySystem* system, String^ role);

private:
    void SelectRow(Object^ sender, DataGridViewCellEventArgs^ e);
    void AddProduct(Object^ sender, EventArgs^ e);
    void UpdateProduct(Object^ sender, EventArgs^ e);
    void DeleteProduct(Object^ sender, EventArgs^ e);
    void RefreshGrid();
    void ClearFields();
    void ClearClicked(Object^ sender, EventArgs^ e);
    void CloseClicked(Object^ sender, EventArgs^ e);
};
