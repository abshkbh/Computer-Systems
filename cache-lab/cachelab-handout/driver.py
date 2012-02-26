#!/usr//bin/python

"""
This wrapper runs the test 3 times and collect results:
  ./test -M 32 -N 32
  ./test -M 64 -N 64
  ./test -M 61 -N 67
"""


import subprocess;
import re;
import os;
import sys;

# compute the score depending on the number of cache misses
def computeMissScore(miss, lower, upper, full_score):
  if miss <= lower:
    return full_score
  if miss >= upper: 
    return 0

  score = (miss - lower) * 1.0 
  range = (upper- lower) * 1.0
  return (1 - score / range) * full_score


def main():
  p = subprocess.Popen("./test -M 32 -N 32 | grep AUTORESULTS", shell=True, stdout=subprocess.PIPE)
  stdout_data = p.communicate()[0]
  result32 = re.findall(r'(\d+)', stdout_data)
  print stdout_data,

  p = subprocess.Popen("./test -M 64 -N 64 | grep AUTORESULTS", shell=True, stdout=subprocess.PIPE)
  stdout_data = p.communicate()[0]
  result64 = re.findall(r'(\d+)', stdout_data)
  print stdout_data,

  p = subprocess.Popen("./test -M 61 -N 67 | grep AUTORESULTS", shell=True, stdout=subprocess.PIPE)
  stdout_data = p.communicate()[0]
  result61 = re.findall(r'(\d+)', stdout_data)
  print stdout_data,

  cachesim_results  = map(int, result32[0:6])
  trans_correctness = int(result32[6]) * int(result64[6]) * int(result61[6]);
  miss32 = int(result32[7])
  miss64 = int(result64[7])
  miss61 = int(result61[7])

  score32 = computeMissScore(miss32, 300, 600, 8) * trans_correctness;
  score64 = computeMissScore(miss64, 1300, 2000, 8) * trans_correctness;
  score61 = computeMissScore(miss61, 2000, 3000, 10) * trans_correctness;

  print "============="
  print "cachesim score: ", cachesim_results
  print "transpose_submit correctness: ", trans_correctness
  print "32*32 matrix transpose misses and score: ", miss32, "(", score32, ")" 
  print "64*64 matrix transpose misses and score: ", miss64, "(", score64, ")"
  print "61*67 matrix transpose misses and score: ", miss61, "(", score61, ")"
  print "============="

  autoresult="%f:%f:%f:%f:%f:%f:%d:%f:%f:%f:%f:%f:%f" % (
      cachesim_results[0],cachesim_results[1],
      cachesim_results[2],cachesim_results[3],
      cachesim_results[4],cachesim_results[5],
      trans_correctness,
      score32,score64, score61,
      miss32, miss64, miss61)


  andrewid = os.environ['USER']

  url='"http://unofficial.fish.ics.cs.cmu.edu/unofficialSubmit.rb?course=15213-f11&user=%s&assessment=cachelab&result=%s"' % (andrewid, autoresult)
  
  if len(sys.argv) == 1:
    print "Posting unofficial score (%s) to Autolab:" % andrewid
    cmd="wget -q -O - -t 5 %s" % (url)
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
    stdout_data = p.communicate()[0]
    print stdout_data

  print "AUTORESULT_STRING=%s" % autoresult


# execute main only if called as a script
if __name__ == "__main__":
  main()

