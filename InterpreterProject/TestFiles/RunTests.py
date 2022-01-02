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
                                    'e=dim[20,20,20]',
                                    'main'])

class EquTests(unittest.TestCase):
    # class variables
    EXEFILE = r'..\x64\Debug\InterpreterProject.exe'
    FILENAME = 'TestFile.tqt'

    def ExecuteTest(self, testLines, expectedOutput):
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

    def ExecuteErrorTest(self, testLines, expectedError):
        # Open a file and add all the test lines to it.
        with open(self.FILENAME, 'w') as f :
            for line in testLines:
                f.write(line + '\n')

        # Execute the file full of statements and capture the output
        s = subprocess.run([self.EXEFILE, '--file', self.FILENAME], capture_output=True).stdout.decode('utf-8')
        results = s.split('\r\n')
        actualError = results[1].strip()
        expectedError = 'FILE:  %s, ' % self.FILENAME + expectedError
        self.assertEqual(actualError, expectedError)

    # lval is an entire array
    def test_equ_lval_array(self):
        self.ExecuteTest(['clear()',
                          'a=dim[10]',
                          'a=7',
                          'print(a)'],
                         ['7'])
        self.ExecuteTest(['clear()',
                          'a=dim[10]',
                          'b=3',
                          'a=b',
                          'print(a)'],
                         ['3'])
        self.ExecuteTest(['clear()',
                          'a=dim[10]',
                          'b=dim[10]',
                          'b[3]=4'
                          'a=b[3]',
                          'print(a)'],
                         ['4'])

        result = ['0'] * 10
        result[8] = '8'
        self.ExecuteTest(['clear()',
                          'a=dim[5]',
                          'b=dim[10]',
                          'b[8]=8'
                          'a=b',
                          'for (i=0,i<10,i=i+1){ print(a[i]) }'],
                         result)
        
        self.ExecuteErrorTest(['clear()',
                          'a=dim[10]',
                          'a=b'],
                         'LINE:  3, COLUMN:  4  b is not defined.')

        self.ExecuteTest(['clear()',
                          'a=dim[10]',
                          'a[1]=4',
                          'print(a[1])'],
                         ['4'])

        self.ExecuteTest(['clear()',
                          'a=dim[10]',
                          'b=3'
                          'a[1]=b',
                          'print(a[1])'],
                         ['3'])

        self.ExecuteTest(['clear()',
                          'a=dim[10]',
                          'b=dim[10]',
                          'b[2]=8',
                          'a[1]=b[2]',
                          'print(a[1])'],
                         ['8'])

        self.ExecuteErrorTest(['clear()',
                          'a=dim[10]',
                          'b=dim[20]',
                          'a[0]=b'],
                         'LINE:  4, COLUMN:  2  a has an unexpected array specifier.')

        self.ExecuteErrorTest(['clear()',
                          'a=dim[10]',
                          'a[0]=b'],
                         'LINE:  3, COLUMN:  7  b is not defined.')

        self.ExecuteErrorTest(['clear()',
                          'a[0]=2'],
                         'LINE:  2, COLUMN:  2  a is not defined.')

        self.ExecuteErrorTest(['clear()',
                          'a=dim[10]',
                          'a[10]=3'],
                         'LINE:  3, COLUMN:  2  a contains an incorrect array specifier.')

        self.ExecuteTest(['clear()',
                          'a=3',
                          'print(a)'],
                         ['3'])

        self.ExecuteTest(['clear()',
                          'a=310',
                          'b=a',
                          'print(b)'],
                         ['310'])

        self.ExecuteTest(['clear()',
                          'a=dim[10]',
                          'a[2]=8'
                          'b=a[2]',
                          'print(b)'],
                         ['8'])

        self.ExecuteTest(['clear()',
                          'a=3',
                          'b=dim[10]',
                          'b[2]=8',
                          'a=b',
                          'print(a[2])'],
                         ['8'])

        self.ExecuteErrorTest(['clear()',
                          'a=b'],
                         'LINE:  2, COLUMN:  4  b is not defined.')

class VarListTests(unittest.TestCase):
    # class variables
    EXEFILE = r'..\x64\Debug\InterpreterProject.exe'
    FILENAME = 'TestFile.tqt'

    def ExecuteTest(self, testLines, expectedOutput):
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

    def ExecuteErrorTest(self, testLines, expectedError):
        # Open a file and add all the test lines to it.
        with open(self.FILENAME, 'w') as f :
            for line in testLines:
                f.write(line + '\n')

        # Execute the file full of statements and capture the output
        s = subprocess.run([self.EXEFILE, '--file', self.FILENAME], capture_output=True).stdout.decode('utf-8')
        results = s.split('\r\n')
        actualError = results[1].strip()
        expectedError = 'FILE:  %s, ' % self.FILENAME + expectedError
        self.assertEqual(actualError, expectedError)

    # Single element
    def test_var_list_single(self):
        self.ExecuteTest(['clear()',
                          '{a}={2}',
                          'print(a)'],
                         ['2'])
        self.ExecuteTest(['clear()',
                          '{b}={3}',
                          'print(b)'],
                         ['3'])

    # Multiple elements
    def test_var_list_multiple(self):
        self.ExecuteTest(['clear()',
                          '{a,b}={2,3}',
                          'print(a,b)'],
                         ['23'])
        self.ExecuteTest(['clear()',
                          '{a,b,c}={3,4,5}',
                          'print(a,b,c)'],
                         ['345'])
    # Arrays
    def test_var_list_arrays(self):
        self.ExecuteTest(['clear()',
                           'a=dim[10]',
                           'a[3]=4',
                           'b=dim[20]',
                           'b[5]=6'
                          '{q,r}={a,b}',
                          'print(q[3],r[5])'],
                         ['46'])

    def test_var_list_errors(self):
        self.ExecuteErrorTest(['clear()',
                               'b=3',
                               'c=4',
                               '{a}={b,c}'],
                               'LINE:  5, COLUMN:  1  1 arguments on left 2 arguments on right.')
        self.ExecuteErrorTest(['clear()',
                               'b=3',
                               'c=4',
                               '{a,b}={c}'],
                               'LINE:  5, COLUMN:  1  2 arguments on left 1 arguments on right.')
        self.ExecuteErrorTest(['clear()',
                               'a=3',
                               'b=4',
                               '{a,b}={a,d}'],
                               'LINE:  4, COLUMN:  11  d is not defined.')

    def test_var_list_functions(self):
        self.ExecuteTest(['clear()',
                          'function foo(){return(1,2,3)}',
                          '{a,b,c}=foo()',
                          'print(a,b,c)'],
                         ['123'])

        self.ExecuteTest(['clear()',
                          'function foo()',
                          '{',
                          '    a=1',
                          '    b=2',
                          '    c=3',
                          '    return(a,b,c)',
                          '}',
                          '{a,b,c}=foo()',
                          'print(a,b,c)'],
                         ['123'])

class SrandRandTests(unittest.TestCase):
    # class variables
    EXEFILE = r'..\x64\Debug\InterpreterProject.exe'
    FILENAME = 'TestFile.tqt'

    def ExecuteTest(self, testLines, expectedOutput):
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

    def test_srand_rand(self):
        self.ExecuteTest(['clear()',
                          'srand(123)',
                          'print(rand())',
                          'print(rand())',
                          'print(rand())'],
                         ['440',
                          '19053',
                          '23075'])
        
        self.ExecuteTest(['clear()',
                          'srand(123)',
                          'a=rand()',
                          'b=rand()',
                          'c=rand()',
                          'print(a)',
                          'print(b)',
                          'print(c)'],
                         ['440',
                          '19053',
                          '23075'])

class LenTests(unittest.TestCase):
    # class variables
    EXEFILE = r'..\x64\Debug\InterpreterProject.exe'
    FILENAME = 'TestFile.tqt'

    def ExecuteTest(self, testLines, expectedOutput):
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

    def ExecuteErrorTest(self, testLines, expectedError):
        # Open a file and add all the test lines to it.
        with open(self.FILENAME, 'w') as f :
            for line in testLines:
                f.write(line + '\n')

        # Execute the file full of statements and capture the output
        s = subprocess.run([self.EXEFILE, '--file', self.FILENAME], capture_output=True).stdout.decode('utf-8')
        results = s.split('\r\n')
        actualError = results[1].strip()
        expectedError = 'FILE:  %s, ' % self.FILENAME + expectedError
        self.assertEqual(actualError, expectedError)

    def test_len_errors(self):
        self.ExecuteErrorTest(['clear()',
                               'a=1',
                               'q=len(a)'],
                               'LINE:  3, COLUMN:  9  a expected entire array.')
        self.ExecuteErrorTest(['clear()',
                               'a=dim[10]',
                               'q=len(a,1)'],
                               'LINE:  3, COLUMN:  11  Valid dimensions for a are 0-0.')
        self.ExecuteErrorTest(['clear()',
                               'a=dim[10,10]',
                               'q=len(a,2)'],
                               'LINE:  3, COLUMN:  11  Valid dimensions for a are 0-1.')
        self.ExecuteErrorTest(['clear()',
                               'a=dim[10,10,10]',
                               'q=len(a,3)'],
                               'LINE:  3, COLUMN:  11  Valid dimensions for a are 0-2.')
    def test_len(self):
        self.ExecuteTest(['clear()',
                          'a=dim[10]',
                          'b=dim[11,12]',
                          'c=dim[13,14,15]',
                          'qa=len(a)',
                          'qb=len(b)',
                          'qc=len(c)',
                          'print(qa,qb,qc)'],
                          ['101113'])
        self.ExecuteTest(['clear()',
                          'a=dim[10]',
                          'b=dim[11,12]',
                          'c=dim[13,14,15]',
                          'qa=len(a,0)',
                          'qb=len(b,0)',
                          'qc=len(c,0)',
                          'print(qa,qb,qc)'],
                          ['101113'])
        self.ExecuteTest(['clear()',
                          'a=dim[10]',
                          'b=dim[11,12]',
                          'c=dim[13,14,15]',
                          'qb=len(b,1)',
                          'qc=len(c,1)',
                          'print(qb,qc)'],
                          ['1214'])
        self.ExecuteTest(['clear()',
                          'a=dim[10]',
                          'b=dim[11,12]',
                          'c=dim[13,14,15]',
                          'qc=len(c,2)',
                          'print(qc)'],
                          ['15'])
     
class ReturnTests(unittest.TestCase):
    # class variables
    EXEFILE = r'..\x64\Debug\InterpreterProject.exe'
    FILENAME = 'TestFile.tqt'

    def ExecuteTest(self, testLines, expectedOutput):
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

    def ExecuteErrorTest(self, testLines, expectedError):
        # Open a file and add all the test lines to it.
        with open(self.FILENAME, 'w') as f :
            for line in testLines:
                f.write(line + '\n')

        # Execute the file full of statements and capture the output
        s = subprocess.run([self.EXEFILE, '--file', self.FILENAME], capture_output=True).stdout.decode('utf-8')
        results = s.split('\r\n')
        actualError = results[1].strip()
        expectedError = 'FILE:  %s, ' % self.FILENAME + expectedError
        self.assertEqual(actualError, expectedError)

    def test_function_return(self):
        self.ExecuteTest(['clear()',
                          'function foo()',
                          '{',
                          '    return(7)',
                          '}',
                          'x=foo()',
                          'print(x)'],
                          ['7'])
        self.ExecuteTest(['clear()',
                          'function foo()',
                          '{',
                          '    return(6,7)',
                          '}',
                          '{x,y}=foo()',
                          'print(x,y)'],
                          ['67'])
        self.ExecuteTest(['clear()',
                          'function foo()',
                          '{',
                          '    a=1',
                          '    b=2',
                          '    return(1,2)',
                          '}',
                          '{x,y}=foo()',
                          'print(x,y)'],
                          ['12'])

    def test_while_return(self):
        self.ExecuteTest(['clear()',
                          'function foo()',
                          '{',
                          '    i=4',
                          '    while(i<10)'
                          '    {',
                          '        return(i)',
                          '    }',
                          '    return(20)',
                          '}',
                          'x=foo()',
                          'print(x)'],
                          ['4'])

        self.ExecuteTest(['clear()',
                          'function foo()',
                          '{',
                          '    i=10',
                          '    while(i<10)'
                          '    {',
                          '        return(i)',
                          '    }',
                          '    return(20)',
                          '}',
                          'x=foo()',
                          'print(x)'],
                          ['20'])

    def test_while_return(self):
        self.ExecuteTest(['clear()',
                          'function foo()',
                          '{',
                          '    i=4',
                          '    while(i<10)'
                          '    {',
                          '        return(i)',
                          '    }',
                          '    return(20)',
                          '}',
                          'x=foo()',
                          'print(x)'],
                          ['4'])

        self.ExecuteTest(['clear()',
                          'function foo()',
                          '{',
                          '    i=10',
                          '    while(i<10)'
                          '    {',
                          '        return(i)',
                          '    }',
                          '    return(20)',
                          '}',
                          'x=foo()',
                          'print(x)'],
                          ['20'])

    def test_for_return(self):
        self.ExecuteTest(['clear()',
                          'function foo()',
                          '{',
                          '    for(i=8,i<10,i=i+1)'
                          '    {',
                          '        return(i)',
                          '    }',
                          '    return(202)',
                          '}',
                          'x=foo()',
                          'print(x)'],
                          ['8'])

        self.ExecuteTest(['clear()',
                          'function foo()',
                          '{',
                          '    for(i=10,i<10,i=i+1)',
                          '    {',
                          '        return(i)',
                          '    }',
                          '    return(202)',
                          '}',
                          'x=foo()',
                          'print(x)'],
                          ['202'])

    def test_if_return(self):
        self.ExecuteTest(['clear()',
                          'function foo()',
                          '{',
                          '    i=0',
                          '    if (i==0)',
                          '    {',
                          '        j=404',
                          '        return(j)',
                          '    }',
                          '    elif (i==1)',
                          '    {',
                          '         j=808',
                          '         return(j)',
                          '    }',
                          '    elif (i==3)',
                          '    {',
                          '         j=909',
                          '         return(j)',
                          '    }',
                          '    elif (i==5)',
                          '    {',
                          '         j=1001',
                          '         return(j)',
                          '    }',
                          '    else',
                          '    {',
                          '        j=2002',
                          '        return(j)'
                          '    }'
                          '    return(202)',
                          '}',
                          'x=foo()',
                          'print(x)'],
                          ['404'])

        self.ExecuteTest(['clear()',
                          'function foo()',
                          '{',
                          '    i=1',
                          '    if (i==0)',
                          '    {',
                          '        j=404',
                          '        return(j)',
                          '    }',
                          '    elif (i==1)',
                          '    {',
                          '         j=808',
                          '         return(j)',
                          '    }',
                          '    elif (i==3)',
                          '    {',
                          '         j=909',
                          '         return(j)',
                          '    }',
                          '    elif (i==5)',
                          '    {',
                          '         j=1001',
                          '         return(j)',
                          '    }',
                          '    else',
                          '    {',
                          '        j=2002',
                          '        return(j)'
                          '    }'
                          '    return(202)',
                          '}',
                          'x=foo()',
                          'print(x)'],
                          ['808'])

        self.ExecuteTest(['clear()',
                          'function foo()',
                          '{',
                          '    i=3',
                          '    if (i==0)',
                          '    {',
                          '        j=404',
                          '        return(j)',
                          '    }',
                          '    elif (i==1)',
                          '    {',
                          '         j=808',
                          '         return(j)',
                          '    }',
                          '    elif (i==3)',
                          '    {',
                          '         j=909',
                          '         return(j)',
                          '    }',
                          '    elif (i==5)',
                          '    {',
                          '         j=1001',
                          '         return(j)',
                          '    }',
                          '    else',
                          '    {',
                          '        j=2002',
                          '        return(j)'
                          '    }'
                          '    return(202)',
                          '}',
                          'x=foo()',
                          'print(x)'],
                          ['909'])

        self.ExecuteTest(['clear()',
                          'function foo()',
                          '{',
                          '    i=5',
                          '    if (i==0)',
                          '    {',
                          '        j=404',
                          '        return(j)',
                          '    }',
                          '    elif (i==1)',
                          '    {',
                          '         j=808',
                          '         return(j)',
                          '    }',
                          '    elif (i==3)',
                          '    {',
                          '         j=909',
                          '         return(j)',
                          '    }',
                          '    elif (i==5)',
                          '    {',
                          '         j=1001',
                          '         return(j)',
                          '    }',
                          '    else',
                          '    {',
                          '        j=2002',
                          '        return(j)'
                          '    }'
                          '    return(202)',
                          '}',
                          'x=foo()',
                          'print(x)'],
                          ['1001'])

        self.ExecuteTest(['clear()',
                          'function foo()',
                          '{',
                          '    i=6',
                          '    if (i==0)',
                          '    {',
                          '        j=404',
                          '        return(j)',
                          '    }',
                          '    elif (i==1)',
                          '    {',
                          '         j=808',
                          '         return(j)',
                          '    }',
                          '    elif (i==3)',
                          '    {',
                          '         j=909',
                          '         return(j)',
                          '    }',
                          '    elif (i==5)',
                          '    {',
                          '         j=1001',
                          '         return(j)',
                          '    }',
                          '    else',
                          '    {',
                          '        j=2002',
                          '        return(j)'
                          '    }'
                          '    return(202)',
                          '}',
                          'x=foo()',
                          'print(x)'],
                          ['2002'])

        self.ExecuteTest(['clear()',
                          'function foo()',
                          '{',
                          '    i=6',
                          '    if (i==0)',
                          '    {',
                          '        j=404',
                          '        return(j)',
                          '    }',
                          '    elif (i==1)',
                          '    {',
                          '         j=808',
                          '         return(j)',
                          '    }',
                          '    elif (i==3)',
                          '    {',
                          '         j=909',
                          '         return(j)',
                          '    }',
                          '    elif (i==5)',
                          '    {',
                          '         j=1001',
                          '         return(j)',
                          '    }',
                          '    else',
                          '    {',
                          '        j=2002',
                          '        k=3003',
                          '    }'
                          '    return(202)',
                          '}',
                          'x=foo()',
                          'print(x)'],
                          ['202'])

class BreakTests(unittest.TestCase):
    # class variables
    EXEFILE = r'..\x64\Debug\InterpreterProject.exe'
    FILENAME = 'TestFile.tqt'

    def ExecuteTest(self, testLines, expectedOutput):
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

    def ExecuteErrorTest(self, testLines, expectedError):
        # Open a file and add all the test lines to it.
        with open(self.FILENAME, 'w') as f :
            for line in testLines:
                f.write(line + '\n')

        # Execute the file full of statements and capture the output
        s = subprocess.run([self.EXEFILE, '--file', self.FILENAME], capture_output=True).stdout.decode('utf-8')
        results = s.split('\r\n')
        actualError = results[1].strip()
        expectedError = 'FILE:  %s, ' % self.FILENAME + expectedError
        self.assertEqual(actualError, expectedError)

    def test_function_break(self):
        self.ExecuteTest(['clear()',
                          'function foo()',
                          '{',
                          '    for (i=0, i<10, i=i+1)',
                          '    {',
                          '        if (i==5)',
                          '        {',
                          '            break',
                          '        }',
                          '    }',
                          '    return(i)'
                          '}',
                          'x=foo()',
                          'print(x)'],
                          ['5'])

        self.ExecuteTest(['clear()',
                          'function foo()',
                          '{',
                          '    i=0',
                          '    while (i<10)',
                          '    {',
                          '        if (i==7)',
                          '        {',
                          '            break',
                          '        }',
                          '        i=i+1',
                          '    }',
                          '    return(i)'
                          '}',
                          'x=foo()',
                          'print(x)'],
                          ['7'])


if __name__ == '__main__':
    unittest.main()
