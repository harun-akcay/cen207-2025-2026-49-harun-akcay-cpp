#!/usr/bin/env python3
"""
Script to convert Markdown report to PDF
Requires: markdown, weasyprint or reportlab
"""

import sys
import os
import subprocess

def check_and_install_package(package_name, import_name=None):
    """Check if package is installed, if not, try to install it"""
    if import_name is None:
        import_name = package_name
    
    try:
        __import__(import_name)
        return True
    except ImportError:
        print(f"Package {package_name} not found. Attempting to install...")
        try:
            subprocess.check_call([sys.executable, "-m", "pip", "install", package_name])
            return True
        except subprocess.CalledProcessError:
            print(f"Failed to install {package_name}. Please install manually:")
            print(f"  pip install {package_name}")
            return False

def convert_markdown_to_pdf_markdown2pdf(md_file, pdf_file):
    """Convert markdown to PDF using markdown2pdf"""
    try:
        from markdown2pdf import convert_markdown_to_pdf
        convert_markdown_to_pdf(md_file, pdf_file)
        return True
    except Exception as e:
        print(f"Error with markdown2pdf: {e}")
        return False

def convert_markdown_to_pdf_weasyprint(md_file, pdf_file):
    """Convert markdown to PDF using markdown + weasyprint"""
    try:
        import markdown
        from weasyprint import HTML, CSS
        from weasyprint.text.fonts import FontConfiguration
        
        # Read markdown file
        with open(md_file, 'r', encoding='utf-8') as f:
            md_content = f.read()
        
        # Convert markdown to HTML
        html_content = markdown.markdown(md_content, extensions=['tables', 'fenced_code', 'codehilite'])
        
        # Add CSS styling
        html_doc = f"""
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset="utf-8">
            <style>
                @page {{
                    size: A4;
                    margin: 2cm;
                }}
                body {{
                    font-family: 'Times New Roman', serif;
                    font-size: 11pt;
                    line-height: 1.6;
                }}
                h1 {{
                    font-size: 18pt;
                    font-weight: bold;
                    margin-top: 1em;
                    margin-bottom: 0.5em;
                    page-break-after: avoid;
                }}
                h2 {{
                    font-size: 14pt;
                    font-weight: bold;
                    margin-top: 0.8em;
                    margin-bottom: 0.4em;
                    page-break-after: avoid;
                }}
                h3 {{
                    font-size: 12pt;
                    font-weight: bold;
                    margin-top: 0.6em;
                    margin-bottom: 0.3em;
                }}
                table {{
                    border-collapse: collapse;
                    width: 100%;
                    margin: 1em 0;
                    page-break-inside: avoid;
                }}
                th, td {{
                    border: 1px solid #ddd;
                    padding: 8px;
                    text-align: left;
                }}
                th {{
                    background-color: #f2f2f2;
                    font-weight: bold;
                }}
                code {{
                    background-color: #f4f4f4;
                    padding: 2px 4px;
                    border-radius: 3px;
                    font-family: 'Courier New', monospace;
                    font-size: 10pt;
                }}
                pre {{
                    background-color: #f4f4f4;
                    padding: 10px;
                    border-radius: 5px;
                    overflow-x: auto;
                    page-break-inside: avoid;
                }}
                hr {{
                    border: none;
                    border-top: 1px solid #ddd;
                    margin: 2em 0;
                }}
                ul, ol {{
                    margin: 0.5em 0;
                    padding-left: 2em;
                }}
                li {{
                    margin: 0.3em 0;
                }}
            </style>
        </head>
        <body>
            {html_content}
        </body>
        </html>
        """
        
        # Convert HTML to PDF
        HTML(string=html_doc).write_pdf(pdf_file)
        return True
    except Exception as e:
        print(f"Error with weasyprint: {e}")
        return False

def convert_markdown_to_pdf_reportlab(md_file, pdf_file):
    """Convert markdown to PDF using markdown + reportlab"""
    try:
        import markdown
        from reportlab.lib.pagesizes import A4
        from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
        from reportlab.lib.units import cm
        from reportlab.platypus import SimpleDocTemplate, Paragraph, Spacer, Table, TableStyle, PageBreak
        from reportlab.lib import colors
        from reportlab.pdfbase import pdfmetrics
        from reportlab.pdfbase.ttfonts import TTFont
        from html.parser import HTMLParser
        import re
        
        # Read markdown file
        with open(md_file, 'r', encoding='utf-8') as f:
            md_content = f.read()
        
        # Convert markdown to HTML
        html_content = markdown.markdown(md_content, extensions=['tables', 'fenced_code'])
        
        # Create PDF
        doc = SimpleDocTemplate(pdf_file, pagesize=A4,
                              rightMargin=2*cm, leftMargin=2*cm,
                              topMargin=2*cm, bottomMargin=2*cm)
        
        styles = getSampleStyleSheet()
        story = []
        
        # Simple HTML to PDF conversion (basic)
        lines = html_content.split('\n')
        for line in lines:
            line = line.strip()
            if not line:
                story.append(Spacer(1, 0.2*cm))
            elif line.startswith('<h1>'):
                text = re.sub(r'<[^>]+>', '', line)
                story.append(Paragraph(text, styles['Heading1']))
            elif line.startswith('<h2>'):
                text = re.sub(r'<[^>]+>', '', line)
                story.append(Paragraph(text, styles['Heading2']))
            elif line.startswith('<h3>'):
                text = re.sub(r'<[^>]+>', '', line)
                story.append(Paragraph(text, styles['Heading3']))
            elif line.startswith('<p>'):
                text = re.sub(r'<[^>]+>', '', line)
                story.append(Paragraph(text, styles['Normal']))
            elif line.startswith('<table'):
                # Skip tables for now (complex to parse)
                continue
            else:
                text = re.sub(r'<[^>]+>', '', line)
                if text:
                    story.append(Paragraph(text, styles['Normal']))
        
        doc.build(story)
        return True
    except Exception as e:
        print(f"Error with reportlab: {e}")
        return False

def main():
    md_file = "MIDTERM_REPORT.md"
    pdf_file = "MIDTERM_REPORT.pdf"
    
    if not os.path.exists(md_file):
        print(f"Error: {md_file} not found!")
        return 1
    
    print(f"Converting {md_file} to {pdf_file}...")
    
    # Try different methods
    methods = [
        ("weasyprint", convert_markdown_to_pdf_weasyprint, ["markdown", "weasyprint"]),
        ("reportlab", convert_markdown_to_pdf_reportlab, ["markdown", "reportlab"]),
    ]
    
    for method_name, method_func, packages in methods:
        print(f"\nTrying {method_name}...")
        
        # Check and install packages
        all_installed = True
        for package in packages:
            if not check_and_install_package(package):
                all_installed = False
                break
        
        if not all_installed:
            continue
        
        # Try to convert
        try:
            if method_func(md_file, pdf_file):
                print(f"✓ Successfully created {pdf_file} using {method_name}")
                return 0
        except Exception as e:
            print(f"✗ {method_name} failed: {e}")
            continue
    
    print("\n✗ All conversion methods failed.")
    print("\nPlease install one of the following:")
    print("  pip install markdown weasyprint")
    print("  OR")
    print("  pip install markdown reportlab")
    print("\nAlternatively, install pandoc:")
    print("  choco install pandoc")
    print("  OR")
    print("  Download from: https://pandoc.org/installing.html")
    
    return 1

if __name__ == "__main__":
    sys.exit(main())

