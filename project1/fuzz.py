import subprocess
import random
import sys

def generate_testcase(charset, length):
	out = ""
	for i in range(length):
		out += random.choice(charset)
	return out

def get_testcase_output(exe_name,inpt):
	p = subprocess.Popen(exe_name,stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
	p.stdin.write(inpt)
	p.stdin.close()
	out = p.stdout.read(30).strip()
	return out

def make_dumpfile(content):
	global numdumps
	f = open("failure_dumps/dump%d"%numdumps,"wb")
	f.write(content)
	f.close()
	numdumps += 1


if __name__ == "__main__":

	global numdumps
	numdumps = 0

	if (len(sys.argv) !=2 ):
		print("usage: ./fuzz.py [NUMTESTS]")
		exit(-1)

	NUMTESTS = int(sys.argv[1])
	CHSET = ["/","*"," ","\n","a","/*","*/"]
	num = 0

	for i in range(NUMTESTS):
		fcontent = generate_testcase (CHSET, random.randint(10,10000))
		out1 = get_testcase_output("./wc209",fcontent)
		out2 = get_testcase_output("./samplewc209",fcontent)

		if(out1 != out2):
			print("[-] testcase%d failed"%i)
			make_dumpfile (fcontent)
		else:
			print("[+] testcase%d successful"%i)
			num+=1

	print("[*] passed %d/%d tests"%(num,NUMTESTS))


