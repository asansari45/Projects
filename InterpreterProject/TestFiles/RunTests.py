import os
import subprocess
import unittest

class FileBasedTests(unittest.TestCase) :

    # Set up in the class method.
    # These are class variables in python.
    FILELOC = r'.'
    EXEFILE = r'..\x64\Debug\InterpreterProject.exe'
    _tstfiles = []

    @classmethod
    def setUpClass(cls):
        for filename in os.listdir(cls.FILELOC):
            f = os.path.join(cls.FILELOC, filename)
            if os.path.isfile(f):
                if '.txt' in f:
                    cls._tstfiles.append(f)

    def ProcessTestOutput(self, f, s):
        testsPassed = None
        testsFailed = None
        print(s)
        lines = s.split('\n')
        for line in lines:
            if 'Successes:' in line:
                testsPassed = int(line.split(':')[1].strip())
            if 'Failures:' in line:
                testsFailed = int(line.split(':')[1].strip())
        self.assertTrue(testsPassed != None)
        self.assertTrue(testsFailed != None)
        self.assertTrue(testsFailed == 0)
        return [testsPassed,testsFailed]

    def test_files(self):
        # iterate through files and execute test.
        totalTestsPassed = 0
        totalTestsFailed = 0
        for f in self._tstfiles:
            print('Running Test File:  %s' % f)
            s = subprocess.run([self.EXEFILE, '--file', f], capture_output=True).stdout.decode('utf-8')
            [testsPassed, testsFailed] = self.ProcessTestOutput(f,s)
            totalTestsPassed += testsPassed
            totalTestsFailed += testsFailed
    
        print('Total Tests Passed:  %d' % totalTestsPassed)
        print('Total Tests Failed:  %d' % totalTestsFailed)

    
class ErrorTests(unittest.TestCase) :

    # class variables
    EXEFILE = r'..\x64\Debug\InterpreterProject.exe'
    FILENAME = 'TestFile.tqt'

    def ExecuteErrorTest(self, testLines, expectedErrors):
        # Open a file and add all the test lines to it.
        with open(self.FILENAME, 'w') as f :
            for line in testLines:
                f.write(line + '\n')

        # Execute the file full of statements and capture the output
        s = subprocess.run([self.EXEFILE, '--file', self.FILENAME], capture_output=True).stdout.decode('utf-8')
        s = s.strip()
        lines = s.split('\n')
        actualErrors = lines[1:]

        # Add the file to the error
        errorPrefix = 'FILE:  %s, ' % self.FILENAME

        if type(expectedErrors) is not list:
            expectedErrors = [expectedErrors]

        # Compare the Errors
        self.assertEqual(len(actualErrors),len(expectedErrors))
        for i in range(len(actualErrors)):
            actualError = actualErrors[i]
            expectedError = errorPrefix + expectedErrors[i]
            print('actual=    (%s)' % actualError)
            print('expected=  (%s)' % expectedError)
            self.assertEqual(actualError, expectedError)

    # Perform a set of tests that make sure the interpreter reports an error
    # These tests execute all the error conditions.
    # ERROR_MISSING_SYMBOL
    def test_missing_symbol(self):
        self.ExecuteErrorTest(['a=b'], 'LINE:  1, COLUMN:  4  b is not defined.')

    # ERROR_INCORRECT_ARRAY_SPECIFIER
    # Incorrect number of dimensions, out of bounds, etc...
    def test_incorrect_array_specifier_as_lval(self):
        self.ExecuteErrorTest(['a=dim[10]', 
                               'a[10]=0'], 
                               'LINE:  2, COLUMN:  2  a contains an incorrect array specifier.')

        self.ExecuteErrorTest(['a=dim[10]', 
                               'a[0,0]=0'], 
                               'LINE:  2, COLUMN:  2  a contains an incorrect array specifier.')

        self.ExecuteErrorTest(['a=dim[10]', 
                               'a[0,0,0]=0'], 
                               'LINE:  2, COLUMN:  2  a contains an incorrect array specifier.')

        self.ExecuteErrorTest(['a=dim[10]',
                               'b=10',
                               'a[b]=0'], 
                               'LINE:  3, COLUMN:  2  a contains an incorrect array specifier.')

        self.ExecuteErrorTest(['a=dim[10,5]', 
                               'a[0,5]=0'], 
                               'LINE:  2, COLUMN:  2  a contains an incorrect array specifier.')

        self.ExecuteErrorTest(['a=dim[10,5]', 
                               'a[0,0,0]=0'], 
                               'LINE:  2, COLUMN:  2  a contains an incorrect array specifier.')

        self.ExecuteErrorTest(['a=dim[10,5]',
                               'b=10',
                               'a[0,b]=0'], 
                               'LINE:  3, COLUMN:  2  a contains an incorrect array specifier.')

        self.ExecuteErrorTest(['a=dim[10,5,3]', 
                               'a[0,0,3]=0'], 
                               'LINE:  2, COLUMN:  2  a contains an incorrect array specifier.')

        self.ExecuteErrorTest(['a=dim[10,5,3]',
                               'b=10',
                               'a[0,0,b]=0'], 
                               'LINE:  3, COLUMN:  2  a contains an incorrect array specifier.')

    def test_incorrect_array_specifier_as_rval(self):
        self.ExecuteErrorTest(['a=dim[10]', 
                               'b=a[10]'], 
                               'LINE:  2, COLUMN:  4  a contains an incorrect array specifier.')

        self.ExecuteErrorTest(['a=dim[10]', 
                               'b=a[0,0]'], 
                               'LINE:  2, COLUMN:  4  a contains an incorrect array specifier.')

        self.ExecuteErrorTest(['a=dim[10]', 
                               'b=a[0,0,0]'], 
                               'LINE:  2, COLUMN:  4  a contains an incorrect array specifier.')

        self.ExecuteErrorTest(['a=dim[10]',
                               'b=10',
                               'c=a[b]'], 
                               'LINE:  3, COLUMN:  4  a contains an incorrect array specifier.')

        self.ExecuteErrorTest(['a=dim[10,5]', 
                               'b=a[0,5]'], 
                               'LINE:  2, COLUMN:  4  a contains an incorrect array specifier.')

        self.ExecuteErrorTest(['a=dim[10,5]', 
                               'b=a[0,0,0]'], 
                               'LINE:  2, COLUMN:  4  a contains an incorrect array specifier.')

        self.ExecuteErrorTest(['a=dim[10,5]',
                               'b=10',
                               'c=a[0,b]'], 
                               'LINE:  3, COLUMN:  4  a contains an incorrect array specifier.')

        self.ExecuteErrorTest(['a=dim[10,5,3]', 
                               'b=a[0,0,3]'], 
                               'LINE:  2, COLUMN:  4  a contains an incorrect array specifier.')

        self.ExecuteErrorTest(['a=dim[10,5,3]',
                               'b=10',
                               'c=a[0,0,b]'], 
                               'LINE:  3, COLUMN:  4  a contains an incorrect array specifier.')

    def test_unexpected_array_specifier_as_lval(self):
        self.ExecuteErrorTest(['a=0', 
                               'a[0]=1'], 
                               'LINE:  2, COLUMN:  2  a has an unexpected array specifier.')

        self.ExecuteErrorTest(['a=0', 
                               'a[0,0]=1'], 
                               'LINE:  2, COLUMN:  2  a has an unexpected array specifier.')

        self.ExecuteErrorTest(['a=0', 
                               'a[0,0,0]=1'], 
                               'LINE:  2, COLUMN:  2  a has an unexpected array specifier.')

        self.ExecuteErrorTest(['a=dim[10]', 
                               'b=dim[10]',
                               'a[0]=b'], 
                               'LINE:  3, COLUMN:  2  a has an unexpected array specifier.')

    def test_unexpected_array_specifier_as_rval(self):
        self.ExecuteErrorTest(['a=0', 
                               'b=a[0]'], 
                               'LINE:  2, COLUMN:  4  a has an unexpected array specifier.')

        self.ExecuteErrorTest(['a=0', 
                               'b=a[0,0]'], 
                               'LINE:  2, COLUMN:  4  a has an unexpected array specifier.')

        self.ExecuteErrorTest(['a=0', 
                               'b=a[0,0,0]'], 
                               'LINE:  2, COLUMN:  4  a has an unexpected array specifier.')

    def test_array_operation_failed_as_lval(self):
        self.ExecuteErrorTest(['a=dim[8]', 
                               'b=dim[9]',
                               'c=a+b'],
                               ['LINE:  3, COLUMN:  4  Array operation failed on array a.  Wrong dimensions or element values of different types.'])

        self.ExecuteErrorTest(['a=dim[9]', 
                               'b=dim[9]',
                               'b[1] = 3.1415927',
                               'c=a+b'],
                               ['LINE:  4, COLUMN:  4  Array operation failed on array a.  Wrong dimensions or element values of different types.'])

        self.ExecuteErrorTest(['a=dim[9]', 
                               'b=dim[9]',
                               'b[1] = \"gadzooks\"',
                               'c=a+b'],
                               ['LINE:  4, COLUMN:  4  Array operation failed on array a.  Wrong dimensions or element values of different types.'])

class FunctionsCommandTests(unittest.TestCase):
    # class variables
    EXEFILE = r'..\x64\Debug\InterpreterProject.exe'
    FILENAME = 'TestFile.tqt'

    def ExecuteFunctionsTest(self, testLines, expectedOutput):
        # Open a file and add all the test lines to it.
        with open(self.FILENAME, 'w') as f :
            for line in testLines:
                f.write(line + '\n')

        # Execute the file full of statements and capture the output
        s = subprocess.run([self.EXEFILE, '--file', self.FILENAME], capture_output=True).stdout.decode('utf-8')
        results = s.split('\r\n')
        results = results[1:-1]
        self.assertEqual(len(results), len(expectedOutput))
        for i in range(len(results)):
            self.assertEqual(results[i], expectedOutput[i])



    def test_functions_command(self):
        self.ExecuteFunctionsTest(['function foo(){}',
                                   'function bar(){}',
                                   'function goo(a,b,c){}',
                                   'functions'],
                                   ['function bar()',
                                    'function foo()',
                                    'function goo(a,b,c)'])

class VarsCommandTests(unittest.TestCase):
    # class variables
    EXEFILE = r'..\x64\Debug\InterpreterProject.exe'
    FILENAME = 'TestFile.tqt'

    def ExecuteFunctionsTest(self, testLines, expectedOutput):
        # Open a file and add all the test lines to it.
        with open(self.FILENAME, 'w') as f :
            for line in testLines:
                f.write(line + '\n')

        # Execute the file full of statements and capture the output
        s = subprocess.run([self.EXEFILE, '--file', self.FILENAME], capture_output=True).stdout.decode('utf-8')
        results = s.split('\r\n')
        results = results[1:-1]
        self.assertEqual(len(results), len(expectedOutput))
        for i in range(len(results)):
            self.assertEqual(results[i], expectedOutput[i])

    def test_functions_command(self):
        self.ExecuteFunctionsTest(['a=1',
                                   'b=3',
                                   'c=dim[10]',
                                   'd=dim[10,10]',
                                   'e=dim[20,20,20]',
                                   'vars'],
                                   ['a',
                                    'b',
                                    'c=dim[10]',
                                    'd=dim[10,10]',
                                    'e=dim[20,20,20]'])

if __name__ == '__main__':
    unittest.main()
