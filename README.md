# Sweet Crumb Bakery — MySQL-connected Visual Studio project

The existing Windows Forms UI layout and the Product/User/Cashier/Manager/BakerySystem OOP structure are retained. Inventory records are loaded from MySQL on application startup. Inventory Add/Update/Delete and completed-sale totals are written to MySQL.

## Requirements
- Visual Studio with **Desktop development with C++**, C++/CLI support, and the v143 toolset (project settings are unchanged).
- MySQL Server and MySQL Workbench.
- MySQL Connector/ODBC 64-bit matching your MySQL installation.

## Setup
1. Open MySQL Workbench and connect to your local MySQL Server.
2. Open and run `database_setup.sql` (lightning bolt). It creates `sweetcrumb_bakery`, `bakery_item`, and `bakery_sale`, and inserts the six original sample items.
3. Install MySQL Connector/ODBC 64-bit if not already installed.
4. Open **ODBC Data Sources (64-bit)** from the Windows Start menu. Go to **System DSN** (or User DSN) → **Add** → choose the installed **MySQL ODBC 9.x Unicode Driver** (driver version/name may differ) → Finish.
5. Configure the DSN with:
   - Data Source Name: `SweetCrumbBakery`
   - TCP/IP Server: `127.0.0.1`
   - Port: `3306` (unless yours differs)
   - User: your MySQL user
   - Database: `sweetcrumb_bakery`
   - Password: enter it in the ODBC configuration; use **Test** to verify.
6. Open `SweetCrumbBakery.sln` in Visual Studio. Select **Debug | x64** and build with **Build → Build Solution**. Run with Ctrl+F5.
7. Login stays as in the original UI: `cashier / 1234` or `manager / 1234`.

## Important notes
- The app uses the Windows ODBC DSN named exactly `SweetCrumbBakery`; it does not store your MySQL password in source code or Git.
- Use a 64-bit ODBC driver because the project is configured for x64.
- If the app reports a database connection error, test the DSN first and confirm MySQL Server is running.
- Sales totals are recorded in `bakery_sale`; stock is updated in `bakery_item`. This simple student project records sale totals, not individual sale-line details.
- This package was modified from the uploaded source, but could not be compiled or run in this environment because Visual Studio, MySQL Server, and the Windows ODBC driver are not available here. Please build/test on the target Windows machine.
