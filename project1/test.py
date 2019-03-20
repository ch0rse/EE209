import subprocess

def get_testcase_output(exe_name,inpt):
	p = subprocess.Popen(exe_name,stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
	p.stdin.write(inpt)
	p.stdin.close()
	out = p.stdout.read(30).strip()
	return out

if __name__ == "__main__":

	TESTS = [0,1,2,3,4,5,7,8,9,12,16,19,20]
	NUMTESTS = len(TESTS)
	num = 0

	for i in TESTS:
		fname = "wc209_sample_tests/test%d.txt"%i
		fp = open(fname,"r")
		fcontent = fp.read()
		fp.close()
		out1 = get_testcase_output("./wc209",fcontent)
		out2 = get_testcase_output("./samplewc209",fcontent)

		if(out1 != out2):
			print("[-] testcase%d failed"%i)
		else:
			print("[+] testcase%d successful"%i)
			num+=1

	print("[*] passed %d/%d tests"%(num,NUMTESTS))


