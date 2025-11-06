# How to Create PDF from Midterm Report

## Method 1: Using HTML File (Easiest - Recommended)

1. Open `MIDTERM_REPORT.html` in your web browser (Chrome, Edge, Firefox)
2. Press `Ctrl + P` (or go to File > Print)
3. Select "Save as PDF" or "Microsoft Print to PDF" as the printer
4. Click "Save" and choose location
5. Done! ✅

## Method 2: Install Pandoc (Best Quality)

### Option A: Using Chocolatey
```bash
# Run PowerShell as Administrator
choco install pandoc
```

### Option B: Using Scoop
```bash
scoop install pandoc
```

### Option C: Manual Installation
1. Download from: https://pandoc.org/installing.html
2. Install the installer
3. Restart terminal

### After Installing Pandoc:
```bash
pandoc MIDTERM_REPORT.md -o MIDTERM_REPORT.pdf --pdf-engine=pdflatex -V geometry:margin=2cm -V fontsize=11pt
```

Or run the batch script:
```bash
generate_pdf_from_markdown.bat
```

## Method 3: Online Converters

1. Go to: https://www.markdowntopdf.com/
2. Upload `MIDTERM_REPORT.md`
3. Download PDF

## Method 4: Using Python (If you have pip)

```bash
pip install markdown weasyprint
python generate_pdf_report.py
```

---

**Note:** The HTML file (`MIDTERM_REPORT.html`) is already created and ready to use with Method 1.

