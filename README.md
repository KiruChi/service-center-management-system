# service-center-management-system
Desktop application for managing mobile device repair orders, built with C++, Qt, and MongoDB. Features role-based access, order tracking, statistics, and repair workflow automation.

## 🛠 Tech Stack

* **Programming Language:** C++17
* **Framework:** Qt 6 (Widgets + Charts)
* **Database:** MongoDB
* **Database Driver:** MongoDB C++ Driver (mongocxx / bsoncxx)
* **Build System:** CMake
* **Package Manager:** vcpkg

## ✨ Key Features

* User authentication with role-based access
* Repair order creation and editing
* Order status workflow management
* Search and filtering of repair orders
* Employee restrictions based on role
* Repair statistics and analytics
* Charts for monthly orders and employee performance
* MongoDB database integration

## 👥 User Roles

### Administrator
* Full access to the system
* Add, edit, and delete orders
* Access to statistics and analytics

### Receptionist
* Register and edit customer orders
* Cannot delete orders
* Limited access to statistics

### Technician
* Update repair process information
* Add diagnostics and completed works
* Cannot create or delete orders

## 📸 Screenshots
<img width="401" height="436" alt="image" src="https://github.com/user-attachments/assets/52746c2c-0d38-4797-b047-0f4ba0abaa35" />
<img width="970" height="712" alt="image" src="https://github.com/user-attachments/assets/e4966cab-bbbc-486a-8c11-84a001796461" />
<img width="404" height="661" alt="image" src="https://github.com/user-attachments/assets/1d1a7c5c-45a3-4cdb-aaa1-e7933cb58257" />
<img width="901" height="757" alt="image" src="https://github.com/user-attachments/assets/0ac37a64-d8ab-4861-945e-dc6ebeba4ed0" />

## ⚙️ Requirements

Before building the project, install:

* Qt 6
* CMake
* MongoDB Community Server
* vcpkg
* Visual Studio with C++ tools


## 📦 Installing Dependencies

Install MongoDB C++ Driver using vcpkg:

```bash
vcpkg install mongo-cxx-driver:x64-windows
```

## 🔧 Build Instructions

### 1. Download repository

### 2. Open project in Qt Creator

Open the folder containing `CMakeLists.txt`.

### 3. Configure CMake

Add CMake option:

```text
CMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
```

Replace the path with your own vcpkg location.

### 4. Run MongoDB

Make sure MongoDB server is running locally:

```text
mongodb://localhost:27017
```

Database used by the application:

```text
ServiceCenter
```

Collections:

```text
Orders
Users
```
Insert test users into the Users collection Example Users:
```bash
use ServiceCenter

db.Users.insertMany([
  {
    name: "admin",
    password: "admin123",
    role: "Administrator"
  },
  {
    name: "reception",
    password: "reception123",
    role: "Receptionist"
  },
  {
    name: "tech",
    password: "tech123",
    role: "Technician"
  }
])
```

### 5. Build and Run

Press:

```text
Build → Run
```

inside Qt Creator.
