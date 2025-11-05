# Project Team and Developer Information

## Project Team

### Instructor
<img title="Dr. Uğur CORUH" src="assets/2023-09-03-17-40-37-image.png" alt="Dr. Uğur CORUH" width="169">

**Dr. Uğur CORUH**  
Asst. Prof. Dr.  
R&D Engineer and System Architecture  
Recep Tayyip Erdoğan University  
Faculty of Engineering and Architecture  
Computer Engineering Department

**Contact:** ugur.coruh@erdogan.edu.tr

---

## Course Information

**Course:** CEN207/CE205 - Data Structures  
**Semester:** Fall 2025-2026  
**Project Number:** 49  
**Project Name:** Simple Inventory Management for Crafters

---

## Project Information

### Project Overview
This project implements a console-based inventory management system for crafters using C programming language. The system demonstrates various data structures and algorithms including:

- Hash Tables (User Authentication)
- Double Linked Lists (Material Inventory)
- Stacks (Login History)
- Binary File Operations

### Project Goals
1. Implement data structures and algorithms from the course curriculum
2. Demonstrate practical application of data structures
3. Achieve 100% test coverage
4. Generate comprehensive documentation
5. Follow software engineering best practices

---

## Development Guidelines

### Code Standards

#### Language
- **Primary Language:** C (C11 standard)
- **Testing Framework:** GoogleTest (C++ for tests)
- **Documentation:** Doxygen (English only)

#### Code Style
- Follow C coding conventions
- Use meaningful variable and function names
- Add comprehensive comments (English only)
- Maintain consistent indentation (4 spaces or tabs)
- Use `astyle` for code formatting

#### Documentation Requirements
- All code must be documented with Doxygen comments
- Documentation must be in **English only**
- 100% documentation coverage required
- Include function descriptions, parameters, return values, and examples

### Project Structure

```
project-root/
├── src/
│   ├── inventory_lib/          # Core library modules
│   │   ├── header/             # Header files
│   │   └── src/                # Source files
│   ├── inventory_app/          # Application layer
│   │   ├── header/
│   │   └── src/
│   └── tests/                  # Test files
│       └── inventory_lib_test/
├── docs/                       # Documentation
├── build_win/                  # Windows build output
├── release_win/                # Release packages
└── assets/                     # Project assets
```

### Build System

#### CMake Configuration
- **Minimum Version:** 3.12
- **C Standard:** C11
- **Build Types:** Debug, Release

#### Build Commands
**Windows:**
```bash
# Full build (documentation + tests + binaries)
7-build-app-windows.bat

# Documentation only
7-build-doc-windows.bat

# Tests only
8-build-test-windows.bat
```

**Linux/WSL:**
```bash
# Full build
./7-build-app-linux.sh
```

### Testing Requirements

#### Test Coverage
- **Target:** 100% code coverage
- **Current:** 99.4% (working towards 100%)
- **Framework:** GoogleTest
- **Reports:** Generated automatically during build

#### Test Guidelines
- All functions must have unit tests
- Tests must run automatically (no user interaction)
- Error paths must be tested
- Use mock systems for error injection

#### Running Tests
```bash
# Windows
cd build_win
ctest --output-on-failure

# Linux
cd build
ctest --output-on-failure
```

### Documentation Generation

#### Doxygen Documentation
```bash
# Generate documentation
doxygen DoxyfileLibWin    # Library documentation
doxygen DoxyfileTestWin   # Test documentation
```

#### Documentation Output
- **HTML:** `docs/doxygenlibwin/html/`
- **PDF:** Generated from HTML (if configured)
- **Coverage Reports:** `docs/coverxygenlibwin/`

### Git Workflow

#### Branch Strategy
- **main:** Stable, production-ready code
- **dev/crafters-inventory-system:** Development branch
- **feature/***: Feature branches
- **documentation:** Documentation updates

#### Commit Guidelines
- Use descriptive commit messages
- Follow conventional commit format (optional)
- Commit related changes together
- Test before committing

#### Pull Request Process
1. Create feature branch from `dev/crafters-inventory-system`
2. Implement changes
3. Ensure tests pass
4. Update documentation
5. Create pull request
6. Code review
7. Merge to dev branch

### Code Review Checklist

Before submitting code for review, ensure:
- [ ] Code compiles without warnings
- [ ] All tests pass
- [ ] Test coverage is maintained or improved
- [ ] Documentation is updated
- [ ] Code follows style guidelines
- [ ] No memory leaks (use valgrind/AddressSanitizer)
- [ ] Error handling is implemented
- [ ] No hardcoded values (use constants)
- [ ] Comments are in English

---

## Development Setup

### Prerequisites

#### Windows
- CMake >= 3.12
- Visual Studio Community Edition (or compatible compiler)
- Git
- Doxygen
- Python (for test result conversion)

#### Linux/WSL
- CMake >= 3.12
- GCC or Clang (C11 support)
- Git
- Doxygen
- Make or Ninja

### Installation Steps

1. **Clone Repository**
   ```bash
   git clone <repository-url>
   cd cen207-2025-2026-49-harun-akcay-cpp
   ```

2. **Initialize Submodules**
   ```bash
   # Windows
   0-init-submodules.bat
   
   # Linux
   git submodule update --init --recursive
   ```

3. **Configure Git Hooks**
   ```bash
   # Windows
   1-configure-git-hooks.bat
   ```

4. **Install Dependencies**
   ```bash
   # Windows
   4-install-windows-enviroment.bat
   
   # Linux/WSL
   ./4-install-wsl-environment.sh
   ```

5. **Build Project**
   ```bash
   # Windows
   7-build-app-windows.bat
   
   # Linux
   ./7-build-app-linux.sh
   ```

---

## Contributing

### Adding New Features

1. **Create Feature Branch**
   ```bash
   git checkout -b feature/feature-name
   ```

2. **Implement Feature**
   - Write code following guidelines
   - Add tests
   - Update documentation

3. **Test Implementation**
   ```bash
   # Run tests
   ctest --output-on-failure
   
   # Check coverage
   # (Coverage reports generated during build)
   ```

4. **Commit Changes**
   ```bash
   git add .
   git commit -m "feat: Add feature description"
   ```

5. **Push and Create PR**
   ```bash
   git push origin feature/feature-name
   ```

### Adding Tests

1. Create test file in `src/tests/inventory_lib_test/`
2. Follow naming convention: `*_test.cpp` or `*_test.c`
3. Use GoogleTest framework
4. Test both success and error paths
5. Ensure 100% coverage for new code

### Updating Documentation

1. Update Doxygen comments in source files
2. Update markdown files in `docs/` directory
3. Regenerate documentation:
   ```bash
   7-build-doc-windows.bat
   ```
4. Verify documentation coverage

---

## Troubleshooting

### Common Issues

#### Build Failures
- **Issue:** CMake not found
  - **Solution:** Install CMake >= 3.12

- **Issue:** Compiler not found
  - **Solution:** Install Visual Studio (Windows) or GCC/Clang (Linux)

#### Test Failures
- **Issue:** Tests not running
  - **Solution:** Ensure GoogleTest is properly configured

- **Issue:** Coverage not generating
  - **Solution:** Check OpenCppCoverage (Windows) or gcov (Linux) installation

#### Documentation Issues
- **Issue:** Doxygen not generating
  - **Solution:** Verify Doxygen installation and configuration files

---

## Resources

### Project Resources
- **Project Guide:** https://ucoruh.github.io/ce205-data-structures/project-guide/
- **Report Template:** https://github.com/rteu-ceng/rteu-ceng-project-homework-report-template
- **Team Selection:** Google Sheets (link in project documentation)

### External Resources
- **CMake Documentation:** https://cmake.org/documentation/
- **Doxygen Documentation:** https://www.doxygen.nl/manual/
- **GoogleTest Documentation:** https://google.github.io/googletest/
- **C11 Standard:** ISO/IEC 9899:2011

---

## Contact

For questions or issues related to the project:
- **Course Instructor:** ugur.coruh@erdogan.edu.tr
- **Project Repository:** GitHub (check repository URL)

---

**Last Updated:** 2025-01-XX  
**Version:** 1.0.0
