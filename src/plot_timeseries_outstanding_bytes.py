#
# Copyright 2018, Arun Saha <arunksaha@gmail.com>
#
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.dates as md
import datetime as dt
import sys
import os

# Open the file, read the string contents into a list,
# and return the list.
def GetLinesListFromFile(filename):
  with open(filename) as f:
    content = f.readlines()
  return content


# Convert usecs (numeric) to datetime
# >>> ts = 1520189090755278 / 1000000.0
# >>> x = datetime.datetime.fromtimestamp(ts)
# >>> x.strftime('%Y-%m-%d %H:%M:%S.%f')
# '2018-03-04 10:44:50.755278'
def ConvertUsecsEpochToDateTime(usecs):
  secs = usecs / 1000000.0
  # Attempt to parse usecs throws:
  #   ValueError: year is out of range
  # So, using secs instead. REVISIT.
  # datetimeObj = dt.datetime.fromtimestamp(usecs)
  datetimeObj = dt.datetime.fromtimestamp(secs)
  # print usecs, secs, datetimeObj
  return datetimeObj


# Take a list of string tuples (timestamp, metric),
# parses them into numerical values and returns
# separate lists.
def GetTxListFromFile(filename):
  lineList = GetLinesListFromFile(filename)
  datetimeList = []
  outBytesList = []

  for line in lineList:
    tokens = line.split()
    # print tokens
    assert(len(tokens) >= 2)
    usecs = int(tokens[0])
    bytes = int(tokens[1])
    datetimeObj = ConvertUsecsEpochToDateTime(usecs)
    datetimeList.append(datetimeObj)
    outBytesList.append(bytes)
  return datetimeList, outBytesList


# Plotting driver program.
def driver(dataFile):

  datetimeList, outBytesList = GetTxListFromFile(dataFile)

  plt.subplots_adjust(bottom = 0.2)
  plt.xticks(rotation = 25)
  ax = plt.gca()
  # Intended to show micro-seconds, but facing some problem,
  # see REVISIT above.
  # xfmt = md.DateFormatter('%Y-%m-%d %H:%M:%S.%f')
  xfmt = md.DateFormatter('%Y-%m-%d %H:%M:%S')
  ax.xaxis.set_major_formatter(xfmt)
  # Avoid scientific notatinn, use plain numbers.
  ax.get_yaxis().get_major_formatter().set_scientific(False)
  # Make the numbers comma separated.
  ax.get_yaxis().set_major_formatter(
    matplotlib.ticker.FuncFormatter(lambda bytes, p: format(int(bytes), ',')))
  # Intended the y-axis numbers on both sides, but not working.
  ax.yaxis.set_ticks_position('both')
  plt.plot(datetimeList, outBytesList)

  plt.title('Outstanding Bytes Timeseries')
  plt.ylabel('bytes')
  plt.xlabel('timestamp')
  plt.grid(True)
  plt.show()


# main
if len(sys.argv) == 1:
  print("usage: {} <input-text-file>".format(sys.argv[0]))
  sys.exit(1)
driver(sys.argv[1])
