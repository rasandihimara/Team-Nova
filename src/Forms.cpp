#include "../include/Forms.h"
#using <System.Data.dll>
using namespace System::Data;
using namespace System::Data::Odbc;


// MySQL access through the Windows ODBC driver. Create a 64-bit ODBC DSN named
// SweetCrumbBakery using the setup instructions in DATABASE_SETUP.md.
static OdbcConnection^ OpenBakeryDatabase()
{
    OdbcConnection^ connection = gcnew OdbcConnection("DSN=SweetCrumbBakery;");
    connection->Open();
    return connection;
}

static void LoadProductsFromDatabase(BakerySystem* system)
{
    OdbcConnection^ connection = OpenBakeryDatabase();
    try
    {
        OdbcCommand^ command = gcnew OdbcCommand(
            "SELECT item_id, item_name, category, unit_price, stock_quantity, reorder_level FROM bakery_item ORDER BY item_id", connection);
        OdbcDataReader^ reader = command->ExecuteReader();
        system->clearProducts();
        while (reader->Read())
        {
            Product product(Convert::ToString(reader[0]) == nullptr ? "" : msclr::interop::marshal_as<std::string>(Convert::ToString(reader[0])),
                msclr::interop::marshal_as<std::string>(Convert::ToString(reader[1])),
                msclr::interop::marshal_as<std::string>(Convert::ToString(reader[2])),
                Convert::ToDouble(reader[3]), Convert::ToInt32(reader[4]), Convert::ToInt32(reader[5]));
            system->addProduct(product);
        }
        reader->Close();
    }
    finally { connection->Close(); }
}

static void ExecuteProductCommand(String^ sql, String^ id, String^ name, String^ category,
                                  double price, int stock, int reorder)
{
    OdbcConnection^ connection = OpenBakeryDatabase();
    try
    {
        OdbcCommand^ command = gcnew OdbcCommand(sql, connection);
        // ODBC uses positional question-mark parameters.
        if (sql->StartsWith("INSERT"))
        {
            command->Parameters->AddWithValue("p1", id);
            command->Parameters->AddWithValue("p2", name);
            command->Parameters->AddWithValue("p3", category);
            command->Parameters->AddWithValue("p4", price);
            command->Parameters->AddWithValue("p5", stock);
            command->Parameters->AddWithValue("p6", reorder);
        }
        else if (sql->StartsWith("UPDATE"))
        {
            command->Parameters->AddWithValue("p1", name);
            command->Parameters->AddWithValue("p2", category);
            command->Parameters->AddWithValue("p3", price);
            command->Parameters->AddWithValue("p4", stock);
            command->Parameters->AddWithValue("p5", reorder);
            command->Parameters->AddWithValue("p6", id);
        }
        else command->Parameters->AddWithValue("p1", id);
        command->ExecuteNonQuery();
    }
    finally { connection->Close(); }
}

static void SaveSaleToDatabase(int orderNumber, double total)
{
    OdbcConnection^ connection = OpenBakeryDatabase();
    try
    {
        OdbcCommand^ command = gcnew OdbcCommand(
            "INSERT INTO bakery_sale (order_number, sale_total) VALUES (?, ?)", connection);
        command->Parameters->AddWithValue("p1", orderNumber);
        command->Parameters->AddWithValue("p2", total);
        command->ExecuteNonQuery();
    }
    finally { connection->Close(); }
}


static String^ ToManaged(const std::string& value)
{
    return gcnew String(value.c_str());
}

static std::string ToNative(String^ value)
{
    return msclr::interop::marshal_as<std::string>(value);
}

AppForm::AppForm(BakerySystem* system, String^ role) : system(system), role(role)
{
    this->BackColor = Color::FromArgb(250, 247, 242);
    this->Font = gcnew Drawing::Font("Segoe UI", 10);
    this->StartPosition = FormStartPosition::CenterScreen;
    this->MinimumSize = Drawing::Size(900, 600);
}

Button^ AppForm::MakeButton(String^ text, int x, int y, int w, int h)
{
    Button^ b = gcnew Button();
    b->Text = text; b->Location = Point(x, y); b->Size = Drawing::Size(w, h);
    StyleButton(b); this->Controls->Add(b); return b;
}

Label^ AppForm::MakeLabel(String^ text, int x, int y, int w, int h, int size)
{
    Label^ l = gcnew Label(); l->Text = text; l->Location = Point(x, y); l->Size = Drawing::Size(w, h);
    l->Font = gcnew Drawing::Font("Segoe UI", (float)size, FontStyle::Regular); this->Controls->Add(l); return l;
}

void AppForm::StyleButton(Button^ button)
{
    button->FlatStyle = FlatStyle::Flat;
    button->BackColor = Color::FromArgb(92, 53, 31);
    button->ForeColor = Color::White;
}

LoginForm::LoginForm(BakerySystem* system) : system(system)
{
    try { LoadProductsFromDatabase(system); }
    catch (Exception^ ex) { MessageBox::Show("Could not connect to MySQL. Follow DATABASE_SETUP.md and check the SweetCrumbBakery ODBC DSN.\n\n" + ex->Message, "Database connection error", MessageBoxButtons::OK, MessageBoxIcon::Error); }
    this->Text = "Sweet Crumb Bakery - Login";
    this->Size = Drawing::Size(520, 430);
    this->StartPosition = FormStartPosition::CenterScreen;
    this->BackColor = Color::FromArgb(250, 247, 242);

    Label^ title = gcnew Label(); title->Text = "Sweet Crumb Bakery"; title->Location = Point(55, 35); title->Size = Drawing::Size(410, 45);
    title->Font = gcnew Drawing::Font("Georgia", 24, FontStyle::Bold); title->ForeColor = Color::FromArgb(85, 47, 28); title->TextAlign = ContentAlignment::MiddleCenter; Controls->Add(title);
    Label^ sub = gcnew Label(); sub->Text = "Staff Login"; sub->Location = Point(55, 82); sub->Size = Drawing::Size(410, 30); sub->TextAlign = ContentAlignment::MiddleCenter; Controls->Add(sub);

    Label^ u = gcnew Label(); u->Text = "Username"; u->Location = Point(75, 130); u->Size = Drawing::Size(120, 25); Controls->Add(u);
    usernameBox = gcnew TextBox(); usernameBox->Location = Point(200, 126); usernameBox->Size = Drawing::Size(220, 28); Controls->Add(usernameBox);
    Label^ p = gcnew Label(); p->Text = "Password"; p->Location = Point(75, 175); p->Size = Drawing::Size(120, 25); Controls->Add(p);
    passwordBox = gcnew TextBox(); passwordBox->Location = Point(200, 171); passwordBox->Size = Drawing::Size(220, 28); passwordBox->UseSystemPasswordChar = true; Controls->Add(passwordBox);
    Label^ r = gcnew Label(); r->Text = "Role"; r->Location = Point(75, 220); r->Size = Drawing::Size(120, 25); Controls->Add(r);
    roleBox = gcnew ComboBox(); roleBox->Location = Point(200, 216); roleBox->Size = Drawing::Size(220, 28); roleBox->DropDownStyle = ComboBoxStyle::DropDownList; roleBox->Items->Add("Cashier"); roleBox->Items->Add("Manager"); roleBox->SelectedIndex = 0; Controls->Add(roleBox);

    Button^ login = gcnew Button(); login->Text = "Login"; login->Location = Point(200, 265); login->Size = Drawing::Size(220, 40); login->BackColor = Color::FromArgb(92, 53, 31); login->ForeColor = Color::White; login->FlatStyle = FlatStyle::Flat; login->Click += gcnew EventHandler(this, &LoginForm::LoginClicked); Controls->Add(login);
    messageLabel = gcnew Label(); messageLabel->Location = Point(75, 320); messageLabel->Size = Drawing::Size(345, 45); messageLabel->TextAlign = ContentAlignment::MiddleCenter; Controls->Add(messageLabel);

    Label^ hint = gcnew Label(); hint->Text = "Demo: cashier / 1234   or   manager / 1234"; hint->Location = Point(60, 365); hint->Size = Drawing::Size(400, 25); hint->TextAlign = ContentAlignment::MiddleCenter; hint->ForeColor = Color::DimGray; Controls->Add(hint);
}

void LoginForm::LoginClicked(Object^ sender, EventArgs^ e)
{
    String^ username = usernameBox->Text->Trim();
    String^ password = passwordBox->Text;
    String^ selectedRole = roleBox->SelectedItem->ToString();

    bool valid = false;
    if (selectedRole == "Cashier") valid = (username == "cashier" && password == "1234");
    else valid = (username == "manager" && password == "1234");

    if (!valid) { messageLabel->Text = "Invalid username, password or role."; messageLabel->ForeColor = Color::Firebrick; return; }

    // Polymorphism: both objects are handled through a User pointer.
    User* user = nullptr;
    if (selectedRole == "Cashier") user = new Cashier("cashier", "1234");
    else user = new Manager("manager", "1234");
    String^ role = ToManaged(user->getRole());
    delete user;

    this->Hide();
    DashboardForm^ dashboard = gcnew DashboardForm(system, role);
    dashboard->FormClosed += gcnew FormClosedEventHandler(this, &LoginForm::DashboardClosed);
    dashboard->Show();
}

void LoginForm::DashboardClosed(Object^ sender, FormClosedEventArgs^ e)
{
    this->Close();
}

DashboardForm::DashboardForm(BakerySystem* system, String^ role) : AppForm(system, role)
{
    this->Text = "Sweet Crumb Bakery - Main Dashboard";
    this->Size = Drawing::Size(1100, 700);

    Label^ title = MakeLabel("Welcome to Sweet Crumb Bakery", 35, 25, 700, 45, 24); title->Font = gcnew Drawing::Font("Segoe UI", 24, FontStyle::Bold); title->ForeColor = Color::FromArgb(72, 42, 27);
    Label^ user = MakeLabel("Logged in as: " + role, 40, 75, 500, 30, 11);
    user->ForeColor = Color::DimGray;

    Button^ sales = MakeButton("Sales Management", 40, 130, 300, 75); sales->Click += gcnew EventHandler(this, &DashboardForm::OpenSales);
    Button^ inventory = MakeButton("Inventory Management", 360, 130, 300, 75); inventory->Click += gcnew EventHandler(this, &DashboardForm::OpenInventory);
    Button^ refresh = MakeButton("Refresh Dashboard", 680, 130, 220, 75); refresh->Click += gcnew EventHandler(this, &DashboardForm::RefreshClicked);

    MakeLabel("Quick Stats", 40, 245, 300, 35, 16)->Font = gcnew Drawing::Font("Segoe UI", 16, FontStyle::Bold);
    MakeLabel("Total Bakery Items", 50, 300, 250, 30, 10); itemValue = MakeLabel("0", 330, 300, 180, 30, 13);
    MakeLabel("Low Stock Items", 50, 345, 250, 30, 10); lowValue = MakeLabel("0", 330, 345, 180, 30, 13);
    MakeLabel("Total Sales (session)", 50, 390, 250, 30, 10); salesValue = MakeLabel("Rs. 0.00", 330, 390, 180, 30, 13);
    MakeLabel("Next Order ID", 50, 435, 250, 30, 10); orderValue = MakeLabel("O6", 330, 435, 180, 30, 13);

    Panel^ info = gcnew Panel(); info->Location = Point(580, 245); info->Size = Drawing::Size(400, 240); info->BackColor = Color::White; Controls->Add(info);
    Label^ infoTitle = gcnew Label(); infoTitle->Text = "System Notes"; infoTitle->Location = Point(20, 20); infoTitle->Size = Drawing::Size(340, 30); infoTitle->Font = gcnew Drawing::Font("Segoe UI", 15, FontStyle::Bold); info->Controls->Add(infoTitle);
    Label^ notes = gcnew Label(); notes->Text = "• No database is used in this beginner version.\n\n• Data is stored in memory while the app is running.\n\n• Manager can add, update and delete inventory.\n\n• Cashier can create sales and reduce stock."; notes->Location = Point(20, 60); notes->Size = Drawing::Size(350, 160); info->Controls->Add(notes);

    Button^ exit = MakeButton("Exit", 40, 540, 140, 40); exit->Click += gcnew EventHandler(this, &DashboardForm::ExitClicked);
    RefreshStats();
}

void DashboardForm::RefreshStats()
{
    itemValue->Text = Convert::ToString((int)system->getProducts().size());
    lowValue->Text = Convert::ToString(system->getLowStockCount());
    salesValue->Text = String::Format("Rs. {0:F2}", system->getTotalSales());
    orderValue->Text = "O" + Convert::ToString(system->getOrderNumber());
}

void DashboardForm::OpenSales(Object^ sender, EventArgs^ e)
{
    SalesForm^ form = gcnew SalesForm(system, role); form->ShowDialog(this); RefreshStats();
}

void DashboardForm::OpenInventory(Object^ sender, EventArgs^ e)
{
    InventoryForm^ form = gcnew InventoryForm(system, role); form->ShowDialog(this); RefreshStats();
}

void DashboardForm::RefreshClicked(Object^ sender, EventArgs^ e)
{
    RefreshStats();
}

void DashboardForm::ExitClicked(Object^ sender, EventArgs^ e)
{
    Close();
}

SalesForm::SalesForm(BakerySystem* system, String^ role) : AppForm(system, role)
{
    this->Text = "Sweet Crumb Bakery - Sales Management";
    this->Size = Drawing::Size(1050, 650);

    Label^ title = MakeLabel("Sales Management", 35, 25, 600, 45, 23); title->Font = gcnew Drawing::Font("Segoe UI", 23, FontStyle::Bold); title->ForeColor = Color::FromArgb(72, 42, 27);
    MakeLabel("Customer", 40, 95, 120, 25, 10); TextBox^ customer = gcnew TextBox(); customer->Location = Point(160, 91); customer->Size = Drawing::Size(250, 28); customer->Text = "Walk-in Customer"; Controls->Add(customer);
    MakeLabel("Payment", 40, 140, 120, 25, 10); ComboBox^ payment = gcnew ComboBox(); payment->Location = Point(160, 136); payment->Size = Drawing::Size(250, 28); payment->Items->Add("Cash"); payment->Items->Add("Card"); payment->Items->Add("Online"); payment->SelectedIndex = 0; Controls->Add(payment);

    MakeLabel("Item", 40, 200, 120, 25, 10); productBox = gcnew ComboBox(); productBox->Location = Point(160, 196); productBox->Size = Drawing::Size(250, 28); productBox->DropDownStyle = ComboBoxStyle::DropDownList; Controls->Add(productBox);
    MakeLabel("Quantity", 40, 245, 120, 25, 10); quantityBox = gcnew NumericUpDown(); quantityBox->Location = Point(160, 241); quantityBox->Size = Drawing::Size(250, 28); quantityBox->Minimum = 1; quantityBox->Maximum = 100; quantityBox->Value = 1; Controls->Add(quantityBox);
    Button^ add = MakeButton("+ Add Item", 160, 290, 250, 40); add->Click += gcnew EventHandler(this, &SalesForm::AddItem);

    saleGrid = gcnew DataGridView(); saleGrid->Location = Point(440, 90); saleGrid->Size = Drawing::Size(550, 300); saleGrid->ReadOnly = true; saleGrid->AllowUserToAddRows = false; saleGrid->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::Fill; Controls->Add(saleGrid);
    saleGrid->Columns->Add("Item", "Item"); saleGrid->Columns->Add("Quantity", "Quantity"); saleGrid->Columns->Add("UnitPrice", "Unit Price"); saleGrid->Columns->Add("Subtotal", "Subtotal");
    Button^ remove = MakeButton("Remove Selected", 440, 410, 180, 40); remove->Click += gcnew EventHandler(this, &SalesForm::RemoveItem);
    totalLabel = MakeLabel("Total: Rs. 0.00", 700, 410, 290, 40, 16); totalLabel->Font = gcnew Drawing::Font("Segoe UI", 16, FontStyle::Bold); totalLabel->TextAlign = ContentAlignment::MiddleRight;
    Button^ complete = MakeButton("Complete Sale", 650, 485, 340, 50); complete->Click += gcnew EventHandler(this, &SalesForm::CompleteSale);
    Button^ close = MakeButton("Close", 40, 530, 140, 40); close->Click += gcnew EventHandler(this, &SalesForm::CloseClicked);

    RefreshProducts(); RefreshSaleGrid();
}

void SalesForm::RefreshProducts()
{
    productBox->Items->Clear();
    for (const auto& p : system->getProducts())
        productBox->Items->Add(String::Format("{0} - {1} (Rs. {2:F2})", ToManaged(p.getId()), ToManaged(p.getName()), p.getPrice()));
    if (productBox->Items->Count > 0) productBox->SelectedIndex = 0;
}

void SalesForm::AddItem(Object^ sender, EventArgs^ e)
{
    if (productBox->SelectedIndex < 0) return;
    String^ selected = productBox->SelectedItem->ToString();
    String^ id = selected->Substring(0, selected->IndexOf(" - "));
    if (!system->addSaleLine(ToNative(id), (int)quantityBox->Value))
        MessageBox::Show("Cannot add item. Check available stock and quantity.", "Sales", MessageBoxButtons::OK, MessageBoxIcon::Warning);
    RefreshSaleGrid();
}

void SalesForm::RefreshSaleGrid()
{
    saleGrid->Rows->Clear();
    for (const auto& line : system->getCurrentSale())
        saleGrid->Rows->Add(ToManaged(line.productName), line.quantity, line.unitPrice.ToString("F2"), line.subtotal().ToString("F2"));
    totalLabel->Text = String::Format("Total: Rs. {0:F2}", system->getSaleTotal());
}

void SalesForm::RemoveItem(Object^ sender, EventArgs^ e)
{
    if (saleGrid->SelectedRows->Count == 0) return;
    int index = saleGrid->SelectedRows[0]->Index;
    system->removeSaleLine(index); RefreshSaleGrid();
}

void SalesForm::CompleteSale(Object^ sender, EventArgs^ e)
{
    if (system->getCurrentSale().empty()) { MessageBox::Show("Add at least one item first."); return; }
    double amount = system->getSaleTotal();
    try
    {
        if (system->completeSale())
        {
            SaveSaleToDatabase(system->getOrderNumber() - 1, amount);
            for (const auto& p : system->getProducts())
                ExecuteProductCommand("UPDATE bakery_item SET item_name=?, category=?, unit_price=?, stock_quantity=?, reorder_level=? WHERE item_id=?", gcnew String(p.getId().c_str()), gcnew String(p.getName().c_str()), gcnew String(p.getCategory().c_str()), p.getPrice(), p.getStock(), p.getReorderLevel());
            MessageBox::Show(String::Format("Sale completed successfully!\nOrder: O{0}\nTotal: Rs. {1:F2}", system->getOrderNumber() - 1, amount), "Success", MessageBoxButtons::OK, MessageBoxIcon::Information);
            RefreshProducts(); RefreshSaleGrid();
        }
    }
    catch (Exception^ ex) { MessageBox::Show("Sale was processed in the application, but saving to MySQL failed. Check the database.\n" + ex->Message, "Database error", MessageBoxButtons::OK, MessageBoxIcon::Error); }
}

void SalesForm::CloseClicked(Object^ sender, EventArgs^ e)
{
    Close();
}

InventoryForm::InventoryForm(BakerySystem* system, String^ role) : AppForm(system, role)
{
    this->Text = "Sweet Crumb Bakery - Inventory Management";
    this->Size = Drawing::Size(1120, 700);

    Label^ title = MakeLabel("Inventory Management", 35, 25, 700, 45, 23); title->Font = gcnew Drawing::Font("Segoe UI", 23, FontStyle::Bold); title->ForeColor = Color::FromArgb(72, 42, 27);
    Label^ access = MakeLabel("Access: " + role, 800, 35, 230, 30, 11); access->TextAlign = ContentAlignment::MiddleRight;

    inventoryGrid = gcnew DataGridView(); inventoryGrid->Location = Point(35, 95); inventoryGrid->Size = Drawing::Size(700, 470); inventoryGrid->ReadOnly = true; inventoryGrid->AllowUserToAddRows = false; inventoryGrid->SelectionMode = DataGridViewSelectionMode::FullRowSelect; inventoryGrid->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::Fill; inventoryGrid->CellClick += gcnew DataGridViewCellEventHandler(this, &InventoryForm::SelectRow); Controls->Add(inventoryGrid);
    inventoryGrid->Columns->Add("ID", "ID"); inventoryGrid->Columns->Add("Name", "Item Name"); inventoryGrid->Columns->Add("Category", "Category"); inventoryGrid->Columns->Add("Price", "Unit Price"); inventoryGrid->Columns->Add("Stock", "Stock"); inventoryGrid->Columns->Add("Reorder", "Reorder Level"); inventoryGrid->Columns->Add("Status", "Status");

    int x = 770;
    MakeLabel("Item ID", x, 100, 120, 25, 10); idBox = gcnew TextBox(); idBox->Location = Point(x + 120, 96); idBox->Size = Drawing::Size(190, 28); Controls->Add(idBox);
    MakeLabel("Item Name", x, 145, 120, 25, 10); nameBox = gcnew TextBox(); nameBox->Location = Point(x + 120, 141); nameBox->Size = Drawing::Size(190, 28); Controls->Add(nameBox);
    MakeLabel("Category", x, 190, 120, 25, 10); categoryBox = gcnew TextBox(); categoryBox->Location = Point(x + 120, 186); categoryBox->Size = Drawing::Size(190, 28); Controls->Add(categoryBox);
    MakeLabel("Price", x, 235, 120, 25, 10); priceBox = gcnew NumericUpDown(); priceBox->Location = Point(x + 120, 231); priceBox->Size = Drawing::Size(190, 28); priceBox->Maximum = 100000; priceBox->DecimalPlaces = 2; priceBox->Increment = 10; Controls->Add(priceBox);
    MakeLabel("Stock", x, 280, 120, 25, 10); stockBox = gcnew NumericUpDown(); stockBox->Location = Point(x + 120, 276); stockBox->Size = Drawing::Size(190, 28); stockBox->Maximum = 100000; Controls->Add(stockBox);
    MakeLabel("Reorder Level", x, 325, 120, 25, 10); reorderBox = gcnew NumericUpDown(); reorderBox->Location = Point(x + 120, 321); reorderBox->Size = Drawing::Size(190, 28); reorderBox->Maximum = 100000; Controls->Add(reorderBox);

    Button^ add = MakeButton("Add", x, 380, 90, 38); add->Click += gcnew EventHandler(this, &InventoryForm::AddProduct);
    Button^ update = MakeButton("Update", x + 100, 380, 100, 38); update->Click += gcnew EventHandler(this, &InventoryForm::UpdateProduct);
    Button^ del = MakeButton("Delete", x + 210, 380, 90, 38); del->BackColor = Color::Firebrick; del->Click += gcnew EventHandler(this, &InventoryForm::DeleteProduct);
    Button^ clear = MakeButton("Clear", x, 430, 300, 38); clear->Click += gcnew EventHandler(this, &InventoryForm::ClearClicked);
    Button^ close = MakeButton("Close", x, 480, 300, 38); close->Click += gcnew EventHandler(this, &InventoryForm::CloseClicked);

    if (role == "Cashier") { add->Enabled = false; update->Enabled = false; del->Enabled = false; }
    RefreshGrid();
}

void InventoryForm::ClearClicked(Object^ sender, EventArgs^ e)
{
    ClearFields();
}

void InventoryForm::CloseClicked(Object^ sender, EventArgs^ e)
{
    Close();
}

void InventoryForm::RefreshGrid()
{
    inventoryGrid->Rows->Clear();
    for (const auto& p : system->getProducts())
    {
        int row = inventoryGrid->Rows->Add(ToManaged(p.getId()), ToManaged(p.getName()), ToManaged(p.getCategory()), String::Format("{0:F2}", p.getPrice()), p.getStock(), p.getReorderLevel(), p.isLowStock() ? "LOW" : "OK");
        if (p.isLowStock()) inventoryGrid->Rows[row]->DefaultCellStyle->BackColor = Color::MistyRose;
    }
}

void InventoryForm::SelectRow(Object^ sender, DataGridViewCellEventArgs^ e)
{
    if (e->RowIndex < 0) return;
    DataGridViewRow^ row = inventoryGrid->Rows[e->RowIndex];
    idBox->Text = row->Cells[0]->Value->ToString(); nameBox->Text = row->Cells[1]->Value->ToString(); categoryBox->Text = row->Cells[2]->Value->ToString();
    priceBox->Value = Decimal::Parse(row->Cells[3]->Value->ToString()); stockBox->Value = Decimal::Parse(row->Cells[4]->Value->ToString()); reorderBox->Value = Decimal::Parse(row->Cells[5]->Value->ToString());
}

void InventoryForm::AddProduct(Object^ sender, EventArgs^ e)
{
    if (idBox->Text->Trim() == "" || nameBox->Text->Trim() == "") { MessageBox::Show("Enter Item ID and Item Name."); return; }
    Product p(ToNative(idBox->Text->Trim()), ToNative(nameBox->Text->Trim()), ToNative(categoryBox->Text->Trim()), (double)priceBox->Value, (int)stockBox->Value, (int)reorderBox->Value);
    if (system->findProduct(p.getId()) != nullptr) { MessageBox::Show("Item ID already exists."); return; }
    try { ExecuteProductCommand("INSERT INTO bakery_item (item_id,item_name,category,unit_price,stock_quantity,reorder_level) VALUES (?,?,?,?,?,?)", idBox->Text->Trim(), nameBox->Text->Trim(), categoryBox->Text->Trim(), (double)priceBox->Value, (int)stockBox->Value, (int)reorderBox->Value); system->addProduct(p); RefreshGrid(); ClearFields(); MessageBox::Show("Item added."); }
    catch (Exception^ ex) { MessageBox::Show("Database insert failed: " + ex->Message); }
}

void InventoryForm::UpdateProduct(Object^ sender, EventArgs^ e)
{
    try { ExecuteProductCommand("UPDATE bakery_item SET item_name=?, category=?, unit_price=?, stock_quantity=?, reorder_level=? WHERE item_id=?", idBox->Text->Trim(), nameBox->Text->Trim(), categoryBox->Text->Trim(), (double)priceBox->Value, (int)stockBox->Value, (int)reorderBox->Value); if (system->updateProduct(ToNative(idBox->Text->Trim()), ToNative(nameBox->Text->Trim()), ToNative(categoryBox->Text->Trim()), (double)priceBox->Value, (int)stockBox->Value, (int)reorderBox->Value)) { RefreshGrid(); MessageBox::Show("Item updated."); } else MessageBox::Show("Item not found."); } catch (Exception^ ex) { MessageBox::Show("Database update failed: " + ex->Message); }
}

void InventoryForm::DeleteProduct(Object^ sender, EventArgs^ e)
{
    if (idBox->Text->Trim() == "") return;
    if (MessageBox::Show("Delete this item?", "Confirm", MessageBoxButtons::YesNo, MessageBoxIcon::Question) == System::Windows::Forms::DialogResult::Yes)
    {
        try { ExecuteProductCommand("DELETE FROM bakery_item WHERE item_id=?", idBox->Text->Trim(), "", "", 0, 0, 0); if (system->deleteProduct(ToNative(idBox->Text->Trim()))) { RefreshGrid(); ClearFields(); } else MessageBox::Show("Item not found."); } catch (Exception^ ex) { MessageBox::Show("Database delete failed: " + ex->Message); }
    }
}

void InventoryForm::ClearFields()
{
    idBox->Clear(); nameBox->Clear(); categoryBox->Clear(); priceBox->Value = 0; stockBox->Value = 0; reorderBox->Value = 0;
}
