#!/usr/bin/env python

import re, os, sys
from reportlab.lib import colors
from reportlab.lib.pagesizes import A4, inch, landscape
from reportlab.platypus import SimpleDocTemplate, Table, TableStyle, Paragraph
from reportlab.lib.styles import getSampleStyleSheet


def buildTable(data):
  doc = SimpleDocTemplate("MOOSE_requirements_tracability.pdf", pagesize=A4, rightMargin=30,leftMargin=30, topMargin=30,bottomMargin=18)
  doc.pagesize = landscape(A4)
  elements = []

  #TODO: Get this line right instead of just copying it from the docs
  style = TableStyle([('ALIGN',(1,1),(-2,-2),'RIGHT'),
                         ('TEXTCOLOR',(1,1),(-2,-2),colors.red),
                         ('VALIGN',(0,0),(0,-1),'TOP'),
                         ('TEXTCOLOR',(0,0),(0,-1),colors.blue),
                         ('ALIGN',(0,-1),(-1,-1),'CENTER'),
                         ('VALIGN',(0,-1),(-1,-1),'MIDDLE'),
                         ('TEXTCOLOR',(0,-1),(-1,-1),colors.green),
                         ('INNERGRID', (0,0), (-1,-1), 0.25, colors.black),
                         ('BOX', (0,0), (-1,-1), 0.25, colors.black),
                         ])

  #Configure style and word wrap
  s = getSampleStyleSheet()
  s = s["BodyText"]
  s.wordWrap = 'CJK'
  data2 = [[Paragraph(cell, s) for cell in row] for row in data]
  t=Table(data2)
  t.setStyle(style)

  #Send the data and build the file
  elements.append(t)
  doc.build(elements)

def extractRequirements(filename):
  f = open(filename)
  text = f.read()
  f.close()

  data = [['Requirement Number', 'Requirement Description']]

  # Extract all of the tables
  table_number = 1
  for table in re.finditer(r'\\begin{tabular}.*?\\end{tabular}', text, re.S):
    if re.search(r'Number.*?Requirement Description', table.group(0)) != None:

      # Extract the requirement
      requirement_number = 0
      for req in re.finditer(r'(\w).*?&\s+(.*?)\s*\\\\', table.group(0)):

        # Skip the header
        if requirement_number > 0:

          # Store the requirment number and description in the data structure
          data.append([req.group(1) + str(table_number) + '.' + str(requirement_number), req.group(2)])

        requirement_number += 1
      table_number += 1

  return data


def extractTestedRequirements():
  # Here we will use the TestHarness to find all of the
  # test files where we can look for tested requirements.
  # Assume SQA docs are located in <MOOSE_DIR>/framework/doc/sqa
  MOOSE_DIR = os.path.abspath(os.path.join('..', '..', '..'))
  #### See if MOOSE_DIR is already in the environment instead
  if os.environ.has_key("MOOSE_DIR"):
    MOOSE_DIR = os.environ['MOOSE_DIR']

  test_app_name = 'moose_test'
  test_app_dir = os.path.join(MOOSE_DIR, 'test')

  # Set the current working directory to test_app_dir
  os.chdir(test_app_dir)

  sys.path.append(os.path.join(MOOSE_DIR, 'python'))
  import path_tool
  path_tool.activate_module('TestHarness')

  from TestHarness import TestHarness
  from Tester import Tester

  # Build the TestHarness object here
  harness = TestHarness(sys.argv, test_app_name, MOOSE_DIR)
  # Tell it to parse the test files only, not run them
  harness.findAndRunTests(find_only=True)

  # Now retrieve all of the accumlated Testers from the TestHarness warehouse
  testers = harness.warehouse.getAllObjects()

  for tester in testers:
    input_file = tester.getInputFile()
    if input_file != None:
      print os.path.join(tester.specs['test_dir'], tester.getInputFile())

      #TODO: Parse the input files for annotations


if __name__ == "__main__":
  data = extractRequirements("SoftwareRequirements.tex")

  extractTestedRequirements()

  buildTable(data)
