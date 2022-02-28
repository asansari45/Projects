import os
import subprocess
import unittest

EXEFILE = r'..\x64\Debug\InterpreterProject.exe'
# EXEFILE = r'..\x64\Release\InterpreterProject.exe'

class FileBasedTests(unittest.TestCase) :

    # Set up in the class method.
    # These are class variables in python.
    FILELOC = r'.'
    LOGFILE = r'InterpreterLog.log'
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

    def ProcessLogForUnfreedMemory(self):
        with open(self.LOGFILE, 'r') as f:
            for line in f:
                self.assertTrue('Unfreed Memory' not in line)

    def test_files(self):
        # iterate through files and execute test.
        totalTestsPassed = 0
        totalTestsFailed = 0
        for f in self._tstfiles:
            print('Running Test File:  %s' % f)
            s = subprocess.run([EXEFILE, '--file', f], capture_output=True).stdout.decode('utf-8')
            [testsPassed, testsFailed] = self.ProcessTestOutput(f,s)
            totalTestsPassed += testsPassed
            totalTestsFailed += testsFailed
            self.ProcessLogForUnfreedMemory()
    
        print('Total Tests Passed:  %d' % totalTestsPassed)
        print('Total Tests Failed:  %d' % totalTestsFailed)

class TestExecutor :
    # class variables
    FILENAME = 'TestFile.tqt'

    def __init__(self, test, testLines, expectedOutputLines, prefixFile=False):
        self._test = test
        self._testLines = testLines
        self._expectedOutputLines = expectedOutputLines
        self._prefixFile = prefixFile
    
    def Execute(self, dbg=False):

        # Open a file and add all the test lines to it.
        with open(self.FILENAME, 'w') as f :
            for line in self._testLines:
                f.write(line + '\n')

        # Execute the file full of statements and capture the output
        s = subprocess.run([EXEFILE, '--file', self.FILENAME], capture_output=True).stdout.decode('utf-8')
        s = s.strip()
        lines = s.split('\r\n')
        actualOutputLines = lines[1:]

        if dbg:
            print('Actual Output:')
            print('\t' + s)

        # Add the file to the prefix
        prefix = ''
        if self._prefixFile :
            prefix = 'FILE:  %s, ' % self.FILENAME

        if type(self._expectedOutputLines) is not list:
            self._expectedOutputLines = [self._expectedOutputLines]

        # Compare the output
        self._test.assertEqual(len(actualOutputLines),len(self._expectedOutputLines))
        for i in range(len(actualOutputLines)):
            actualOutputLine = actualOutputLines[i]
            expectedOutputLine = prefix + self._expectedOutputLines[i]
            # print('actual=    (%s)' % actualError)
            # print('expected=  (%s)' % expectedError)
            self._test.assertEqual(actualOutputLine, expectedOutputLine)


class ErrorTests(unittest.TestCase) :

    def ExecuteErrorTest(self, testLines, expectedErrors):
        TestExecutor(self, testLines, expectedErrors, True).Execute()

    # Perform a set of tests that make sure the interpreter reports an error
    # These tests execute all the error conditions.
    # ERROR_MISSING_SYMBOL
    def test_missing_symbol(self):
        # self.ExecuteErrorTest(['a=b'], 'LINE:  1, COLUMN:  4  b is not defined.')
        TestExecutor(self, ['a=b'], 'LINE:  1, COLUMN:  4  b is not defined.', True).Execute()

    # ERROR_INCORRECT_ARRAY_SPECIFIER
    # Incorrect number of dimensions, out of bounds, etc...
    def test_incorrect_array_specifier_as_lval(self):
        TestExecutor(self, ['a=dim[10]', 
                            'a[10]=0'], 
                            'LINE:  2, COLUMN:  2  a contains an incorrect array specifier.',
                            True).Execute()

        TestExecutor(self, ['a=dim[10]', 
                            'a[0,0]=0'], 
                            'LINE:  2, COLUMN:  2  a contains an incorrect array specifier.',
                            True).Execute()

        TestExecutor(self, ['a=dim[10]', 
                            'a[0,0,0]=0'], 
                            'LINE:  2, COLUMN:  2  a contains an incorrect array specifier.',
                            True).Execute()

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
                               ['LINE:  3, COLUMN:  4  Array operation failed on array.  Wrong dimensions or element values of different types.'])

        self.ExecuteErrorTest(['a=dim[9]', 
                               'b=dim[9]',
                               'b[1] = \"gadzooks\"',
                               'c=a+b'],
                               ['LINE:  4, COLUMN:  4  Array operation failed on array.  Wrong dimensions or element values of different types.'])

class FunctionsCommandTests(unittest.TestCase):

    def test_functions_command(self):
        TestExecutor(self, ['function foo(){}',
                            'function bar(){}',
                            'function goo(a,b,c){}',
                            '.functions'],
                           ['function bar()',
                            'function foo()',
                            'function goo(a,b,c)'], False).Execute()

class VarsCommandTests(unittest.TestCase):
    def test_functions_command(self):
        TestExecutor(self, ['a=1',
                            'b=3',
                            'c=dim[10]',
                            'd=dim[10,10]',
                            'e=dim[5,5,5]',
                            '.vars'],
                            ['GLOBAL Symbol Table Contents',
                            'a  1',
                            'b  3',
                            'c=dim[10]  [0...9]:  0 0 0 0 0 0 0 0 0 0 ...',
                            'd=dim[10,10]  [0,0...9]:  0 0 0 0 0 0 0 0 0 0 ...',
                            'e=dim[5,5,5]  [0,0,0...9]:  0 0 0 0 0 ...',
                            'main  0'], False).Execute()

class EquTests(unittest.TestCase):
    def ExecuteTest(self, testLines, expectedOutput):
        TestExecutor(self, testLines, expectedOutput, False).Execute()

    def ExecuteErrorTest(self, testLines, expectedError):
        TestExecutor(self, testLines, expectedError, True).Execute()
        
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
    def ExecuteTest(self, testLines, expectedOutput):
        TestExecutor(self, testLines, expectedOutput).Execute()

    def ExecuteErrorTest(self, testLines, expectedError):
        TestExecutor(self, testLines, expectedError, True).Execute()

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

    # Expressions
    def test_var_list_expressions(self):
        self.ExecuteTest(['clear()',
                           'a=5',
                           'b=6',
                           '{a,b}={a+b,b-a}',
                           'print(a,b)'],
                         ['111'])


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
    def ExecuteTest(self, testLines, expectedOutput):
        TestExecutor(self, testLines, expectedOutput).Execute()

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
    def ExecuteTest(self, testLines, expectedOutput):
        TestExecutor(self, testLines, expectedOutput).Execute()

    def ExecuteErrorTest(self, testLines, expectedError):
        TestExecutor(self, testLines, expectedError, True).Execute()

    def test_len_errors(self):
        self.ExecuteErrorTest(['clear()',
                               'a=1',
                               'q=len(a)'],
                               'LINE:  3, COLUMN:  9  Expected entire array.')
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
                          'c=dim[3,4,5]',
                          'qa=len(a)',
                          'qb=len(b)',
                          'qc=len(c)',
                          'print(qa,qb,qc)'],
                          ['10113'])
        self.ExecuteTest(['clear()',
                          'a=dim[10]',
                          'b=dim[11,12]',
                          'c=dim[3,4,5]',
                          'qa=len(a,0)',
                          'qb=len(b,0)',
                          'qc=len(c,0)',
                          'print(qa,qb,qc)'],
                          ['10113'])
        self.ExecuteTest(['clear()',
                          'a=dim[10]',
                          'b=dim[11,12]',
                          'c=dim[3,4,5]',
                          'qb=len(b,1)',
                          'qc=len(c,1)',
                          'print(qb,qc)'],
                          ['124'])
        self.ExecuteTest(['clear()',
                          'a=dim[10]',
                          'b=dim[11,12]',
                          'c=dim[3,4,5]',
                          'qc=len(c,2)',
                          'print(qc)'],
                          ['5'])
     
class ReturnTests(unittest.TestCase):
    def ExecuteTest(self, testLines, expectedOutput):
        TestExecutor(self, testLines, expectedOutput).Execute()

    def ExecuteErrorTest(self, testLines, expectedError):
        TestExecutor(self, testLines, expectedError, True).Execute()


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
    def ExecuteTest(self, testLines, expectedOutput):
        TestExecutor(self, testLines, expectedOutput).Execute()

    def ExecuteErrorTest(self, testLines, expectedError):
        TestExecutor(self, testLines, expectedError, True).Execute()


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

class StackFunctionsTests(unittest.TestCase):
    def ExecuteTest(self, testLines, expectedOutput):
        TestExecutor(self, testLines, expectedOutput).Execute()

    def ExecuteErrorTest(self, testLines, expectedError):
        TestExecutor(self, testLines, expectedError, True).Execute()


    def test_stack_init(self):
        self.ExecuteTest(['clear()',
                          'load(..\Functions\StackFunctions.txt)',
                          'mystack=StackInit(10)',
                          'mytop=0',
                          'print(len(mystack),mytop)'],
                          ['Parsing from file:  ..\Functions\StackFunctions.txt', 
                           '100'])
        self.ExecuteTest(['clear()',
                          'load(..\Functions\StackFunctions.txt)',
                          'mystack=StackInit(100)',
                          'mytop=0',
                          'print(len(mystack),mytop)'],
                          ['Parsing from file:  ..\Functions\StackFunctions.txt', 
                           '1000'])

    def test_stack_push(self):
        self.ExecuteTest(['clear()',
                          'load(..\Functions\StackFunctions.txt)',
                          'mystack=StackInit(10)',
                          'mytop=0',
                          'status=StackPush(mystack,mytop,1)',
                          'status=StackPush(mystack,mytop,2)',
                          'status=StackPush(mystack,mytop,3)',
                          'status=StackPush(mystack,mytop,5)',
                          'status=StackPush(mystack,mytop,7)',
                          'status=StackPush(mystack,mytop,11)',
                          'print(mytop)',
                          'status=StackDump(mystack,mytop)'
                          ],
                          ['Parsing from file:  ..\Functions\StackFunctions.txt', 
                           '6',
                           '0.  1',
                           '1.  2',
                           '2.  3',
                           '3.  5',
                           '4.  7',
                           '5.  11',
                           'STACK ITEM COUNT:  6',
                           'STACK SIZE:        10'])

        self.ExecuteTest(['clear()',
                          'load(..\Functions\StackFunctions.txt)',
                          'mystack=StackInit(10)',
                          'mytop=0',
                          'status=StackPush(mystack,mytop,1)',
                          'status=StackPush(mystack,mytop,2)',
                          'status=StackPush(mystack,mytop,3)',
                          'status=StackPush(mystack,mytop,5)',
                          'status=StackPush(mystack,mytop,7)',
                          'status=StackPush(mystack,mytop,11)',
                          'status=StackPush(mystack,mytop,13)',
                          'status=StackPush(mystack,mytop,17)',
                          'status=StackPush(mystack,mytop,19)',
                          'status=StackPush(mystack,mytop,23)',
                          'print(mytop)',
                          'status=StackDump(mystack,mytop)'
                          ],
                          ['Parsing from file:  ..\Functions\StackFunctions.txt', 
                           '10',
                           '0.  1',
                           '1.  2',
                           '2.  3',
                           '3.  5',
                           '4.  7',
                           '5.  11',
                           '6.  13',
                           '7.  17',
                           '8.  19',
                           '9.  23',
                           'STACK ITEM COUNT:  10',
                           'STACK SIZE:        10'])

        self.ExecuteTest(['clear()',
                          'load(..\Functions\StackFunctions.txt)',
                          'mystack=StackInit(10)',
                          'mytop=0',
                          'status=StackPush(mystack,mytop,1)',
                          'status=StackPush(mystack,mytop,2)',
                          'status=StackPush(mystack,mytop,3)',
                          'status=StackPush(mystack,mytop,5)',
                          'status=StackPush(mystack,mytop,7)',
                          'status=StackPush(mystack,mytop,11)',
                          'status=StackPush(mystack,mytop,13)',
                          'status=StackPush(mystack,mytop,17)',
                          'status=StackPush(mystack,mytop,19)',
                          'status=StackPush(mystack,mytop,23)',
                          'print(mytop)',
                          'status=StackDump(mystack,mytop)'
                          ],
                          ['Parsing from file:  ..\Functions\StackFunctions.txt', 
                           '10',
                           '0.  1',
                           '1.  2',
                           '2.  3',
                           '3.  5',
                           '4.  7',
                           '5.  11',
                           '6.  13',
                           '7.  17',
                           '8.  19',
                           '9.  23',
                           'STACK ITEM COUNT:  10',
                           'STACK SIZE:        10'])

        self.ExecuteTest(['clear()',
                          'load(..\Functions\StackFunctions.txt)',
                          'mystack=StackInit(10)',
                          'mytop=0',
                          'status=StackPush(mystack,mytop,1)',
                          'status=StackPush(mystack,mytop,2)',
                          'status=StackPush(mystack,mytop,3)',
                          'status=StackPush(mystack,mytop,5)',
                          'status=StackPush(mystack,mytop,7)',
                          'status=StackPush(mystack,mytop,11)',
                          'status=StackPush(mystack,mytop,13)',
                          'status=StackPush(mystack,mytop,17)',
                          'status=StackPush(mystack,mytop,19)',
                          'status=StackPush(mystack,mytop,23)',
                          'status=StackPush(mystack,mytop,29)',
                          'print(status)',
                          'print(mytop)',
                          'status=StackDump(mystack,mytop)'
                          ],
                          ['Parsing from file:  ..\Functions\StackFunctions.txt', 
                          '0',
                           '10',
                           '0.  1',
                           '1.  2',
                           '2.  3',
                           '3.  5',
                           '4.  7',
                           '5.  11',
                           '6.  13',
                           '7.  17',
                           '8.  19',
                           '9.  23',
                           'STACK ITEM COUNT:  10',
                           'STACK SIZE:        10'])

    def test_stack_pop(self):
        self.ExecuteTest(['clear()',
                          'load(..\Functions\StackFunctions.txt)',
                          'mystack=StackInit(10)',
                          'mytop=0',
                          'status=StackPush(mystack,mytop,1)',
                          'status=StackPush(mystack,mytop,2)',
                          'status=StackPush(mystack,mytop,3)',
                          'status=StackPush(mystack,mytop,5)',
                          'status=StackPush(mystack,mytop,7)',
                          'status=StackPush(mystack,mytop,11)',
                          '{status,myitem}=StackPop(mystack,mytop)',
                          'print(myitem)',
                          'status=StackDump(mystack,mytop)'
                          ],
                          ['Parsing from file:  ..\Functions\StackFunctions.txt', 
                           '11',
                           '0.  1',
                           '1.  2',
                           '2.  3',
                           '3.  5',
                           '4.  7',
                           'STACK ITEM COUNT:  5',
                           'STACK SIZE:        10'])
        
        self.ExecuteTest(['clear()',
                          'load(..\Functions\StackFunctions.txt)',
                          'mystack=StackInit(10)',
                          'mytop=0',
                          'status=StackPush(mystack,mytop,1)',
                          'status=StackPush(mystack,mytop,2)',
                          'status=StackPush(mystack,mytop,3)',
                          'status=StackPush(mystack,mytop,5)',
                          'status=StackPush(mystack,mytop,7)',
                          'status=StackPush(mystack,mytop,11)',
                          '{status,myitem}=StackPop(mystack,mytop)',
                          'print(myitem)',
                          '{status,myitem}=StackPop(mystack,mytop)',
                          'print(myitem)',
                          'StackDump(mystack,mytop)'
                          ],
                          ['Parsing from file:  ..\Functions\StackFunctions.txt', 
                           '11',
                           '7',
                           '0.  1',
                           '1.  2',
                           '2.  3',
                           '3.  5',
                           'STACK ITEM COUNT:  4',
                           'STACK SIZE:        10'])

        self.ExecuteTest(['clear()',
                          'load(..\Functions\StackFunctions.txt)',
                          'mystack=StackInit(10)',
                          'mytop=0',
                          '{status,myitem}=StackPop(mystack,mytop)',
                          'print(status)',
                          'print(myitem)',
                          'print(mytop)',
                          ],
                          ['Parsing from file:  ..\Functions\StackFunctions.txt', 
                           '0',
                           '-1',
                           '0'])

class ReferenceTests(unittest.TestCase):
    def ExecuteTest(self, testLines, expectedOutput):
        TestExecutor(self, testLines, expectedOutput).Execute()

    def ExecuteErrorTest(self, testLines, expectedError):
        TestExecutor(self, testLines, expectedError, True).Execute()


    def test_simple_references(self):
        # simple variable
        self.ExecuteTest(['clear()',
                          'function foo(&a)',
                          '{',
                          '    a=3',
                          '}',
                          'b=8',
                          'foo(b)',
                          'print(b)'],
                          ['3'])
        
        # array variables
        self.ExecuteTest(['clear()',
                          'function foo(&a)',
                          '{',
                          '    a[8]=130',
                          '}',
                          'b=dim[10]',
                          'foo(b)',
                          'print(b[8])'],
                          ['130'])
        self.ExecuteTest(['clear()',
                          'function foo(&a)',
                          '{',
                          '    a[0,8]=180',
                          '}',
                          'b=dim[10,10]',
                          'foo(b)',
                          'print(b[0,8])'],
                          ['180'])
        self.ExecuteTest(['clear()',
                          'function foo(&a)',
                          '{',
                          '    a[3,1,9]=428',
                          '}',
                          'b=dim[10,10,10]',
                          'foo(b)',
                          'print(b[0,0,0],b[3,1,9])'],
                          ['0428'])

    def test_mixture_references(self):
        self.ExecuteTest(['clear()',
                          'function foo(&a,b)',
                          '{',
                          '    a=b+5',
                          '}',
                          'b=10',
                          'foo(b,b)',
                          'print(b)'],
                          ['15'])
        self.ExecuteTest(['clear()',
                          'function foo(&a,b,c,&d)',
                          '{',
                          '    a=b+5+c',
                          '    d=b+20',
                          '}',
                          'q=5',
                          'r=10',
                          's=20',
                          't=25',
                          'foo(q,r,s,t)',
                          'print(q,r,s,t)'],
                          ['35102030'])

    def test_nested_references(self):
        self.ExecuteTest(['clear()',
                          'function foo(&a)',
                          '{',
                          '    a=a+5',
                          '}',
                          'function goo(&a)',
                          '{',
                          '    a=a+5'
                          '    foo(a)',
                          '}',
                          'b=10',
                          'goo(b)',
                          'print(b)'],
                          ['20'])
        self.ExecuteTest(['clear()',
                          'function foo(&a)',
                          '{',
                          '    a=a+5',
                          '}',
                          'function goo(&a)',
                          '{',
                          '    a=a+5'
                          '    foo(a)',
                          '}',
                          'function hoo(&a)',
                          '{',
                          '    a=a+5'
                          '    goo(a)',
                          '}',
                          'b=10',
                          'hoo(b)',
                          'print(b)'],
                          ['25'])

    def test_return_references(self):
        self.ExecuteTest(['clear()',
                          'function foo(&a)',
                          '{',
                          '    a=a+5',
                          '    return(a)',
                          '}',
                          'b=10',
                          'x = foo(b)',
                          'print(b,x)'],
                          ['1515'])

class RadixSortTests(unittest.TestCase):
    def ExecuteTest(self, testLines, expectedOutput):
        TestExecutor(self, testLines, expectedOutput).Execute()

    def ExecuteErrorTest(self, testLines, expectedError):
        TestExecutor(self, testLines, expectedError, True).Execute()


    def test_radix_sort(self):
        self.ExecuteTest(['clear()',
                          'load(..\Functions\RadixSort.txt)',
                          'array=dim[10]',
                          'array[0]=5400',
                          'array[1]=405',
                          'array[2]=2',
                          'array[3]=3000',
                          'array[4]=6',
                          'array[5]=5',
                          'array[6]=4',
                          'array[7]=3',
                          'array[8]=400',
                          'array[9]=300',
                          'status=RadixSort(array)',
                          'print(status)',
                          'for (i = 0, i < 10, i=i+1)',
                          '{',
                          '    print(array[i])',
                          '}'],
                          ['Parsing from file:  ..\Functions\RadixSort.txt',
                           '1',
                           '2',
                           '3',
                           '4',
                           '5',
                           '6',
                           '300',
                           '400',
                           '405',
                           '3000',
                           '5400'])

class PerformanceTests(unittest.TestCase):
    def ExecuteTest(self, testLines, expectedOutput):
        TestExecutor(self, testLines, expectedOutput).Execute()

    def ExecuteErrorTest(self, testLines, expectedError):
        TestExecutor(self, testLines, expectedError, True).Execute()

    def test_performance(self):
        commands = ['clear()']
        commands += 'x=0'
        commands.extend(['x=x+1'] * 1000)
        commands.extend(['print(x)'])
        self.ExecuteTest(commands,
                          ['1000'])

class FileIoTests(unittest.TestCase):

    def test_fopen_fclose(self):
        testLines = ['clear()', 
                     'a=4',
                     'f=fopen(\"diver.bin\", \"w\")',
                     'fwrite(f, a)',
                     'fclose(f)',
                     'clear()',
                     'f=fopen(\"diver.bin\", \"r\")',
                     'fread(f,b)',
                     'fclose(f)',
                     'print(b)']
        expectedLines = ['4']
        TestExecutor(self, testLines, expectedLines).Execute(dbg=True)

if __name__ == '__main__':
    unittest.main()
