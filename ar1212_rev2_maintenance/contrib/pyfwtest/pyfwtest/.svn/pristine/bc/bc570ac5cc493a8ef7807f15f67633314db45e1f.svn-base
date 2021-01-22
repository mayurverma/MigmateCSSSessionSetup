####################################################################################
# Copyright 2018 ON Semiconductor.  All rights reserved.
#
# This software and/or documentation is licensed by ON Semiconductor under limited
# terms and conditions. The terms and conditions pertaining to the software and/or
# documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
# ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
# Do not use this software and/or documentation unless you have carefully read and
# you agree to the limited terms and conditions. By using this software and/or
# documentation, you agree to the limited terms and conditions.
#
####################################################################################
# Firmware test framework results reporter classes
####################################################################################

import io
import logging
import traceback
import unittest
import xml.dom.minidom

from .case import TestCase
from .decorators import WritelnDecorator
from .listeners import ITestResultListener
from .utils import CompareTestIds

class ReporterBase(ITestResultListener):
    """A base class for classes that report test results."""

    def _countRelevantTraceBackLevels(self, tb):
        length = 0
        while tb and not self._isRelevantTraceBackLevel(tb):
            length += 1
            tb = tb.tb_next
        return length

    def _isRelevantTraceBackLevel(self, tb):
        return '__unittest' in tb.tb_frame.f_globals

    def _excInfoString(self, err, test):
        """Converts a sys.exc_info()-style tuple of values into a string."""
        exctype, value, tb = err
        # Skip test runner traceback levels
        while tb and self._isRelevantTraceBackLevel(tb):
            tb = tb.tb_next
        if exctype is test.failureException:
            # Skip assert*() traceback levels
            length = self._countRelevantTraceBackLevels(tb)
            return ''.join(traceback.format_exception(exctype, value,
                                                      tb, length))
        return ''.join(traceback.format_exception(exctype, value, tb))

    def _errorHolderInfo(self, obj):
        """Converts a unittest._ErrorHolder description to module, class, method tuple."""
        methodName, fields = str(obj).split()
        moduleName, className = fields.strip('(').rstrip(')').split('.')
        return (moduleName, className, methodName)

class HtmlTestReporterBase(ReporterBase):
    """A test result base class that can write test results to a file in XHTML format."""

    def __init__(self, title, filename):
        super().__init__()
        self._title = title
        self._filename = filename

    def _write(self):
        """Write the report file."""
        logger = logging.getLogger(self.__class__.__name__)
        logger.info('Generating report file : ' + self._filename)
        try:
            stream = open(self._filename, 'wt')
            try:
                logger.debug('Writing contents')
                stream.write(self._strbuf.getvalue())
            except IOError as e:
                logger.warn('Error writing file : ' + str(e))
            finally:
                stream.close()
                self._strbuf.close()
                logger.debug('Closed file')
        except IOError as err:
            logger.warn('Error opening file : ' + str(err))

    def startTestRun(self, epoch, fixture, suite):
        """Called once before any tests are executed."""
        self._strbuf = io.StringIO()
        self._strbuf.write('<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">\n')
        self._strbuf.write('<html>\n<head>\n<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />\n')
        self._strbuf.write('<title>' + self._title + '</title>\n')
        self._strbuf.write('<style type="text/css">\n')
        self._strbuf.write('div {display:block; font-weight:normal; font-family:Geneva, Arial, Helvetica, sans-serif;}\n')
        self._strbuf.write('table {border:2px #7f7f7f solid; border-collapse:collapse;}\n')
        self._strbuf.write('th {border:2px #7f7f7f solid;}\n')
        self._strbuf.write('td {border:2px #7f7f7f solid;}\n')
        self._strbuf.write('pre {font-size:70%;}\n')
        self._strbuf.write('.skip {background-color:MediumPurple;}\n')
        self._strbuf.write('.pass {background-color:LimeGreen;}\n')
        self._strbuf.write('.error {background-color:Cyan;}\n')
        self._strbuf.write('.fail {background-color:Red;}\n')
        self._strbuf.write('.wfail {background-color:Orange;}\n')
        self._strbuf.write('.efail {background-color:Magenta;}\n')
        self._strbuf.write('.upass {background-color:GreenYellow;}\n')
        self._strbuf.write('</style>\n</head>\n<body>\n')
        self._strbuf.write('<h1>' + self._title + '</h1>\n')

    def stopTestRun(self, epoch, result):
        """Called once after all tests are executed."""
        self._strbuf.write('</body>\n</html>\n')
        self._write()


class HtmlTestSummaryReporter(HtmlTestReporterBase):
    """A test result class that can write a test result summary to a file in XHTML format."""

    def __init__(self, filename):
        super().__init__('Test Summary', filename)
        self._classes = {}
        self._tests = {}

    def _incrementCount(self, test, result_type):
        """Increments the summary count for the given class and result type"""
        className = test.__class__.__name__
        if className not in self._tests:
            self._tests[className] = {'pass': 0, 'error': 0, 'fail': 0, 'upass' : 0}
            self._classes[test.getTestIdentifier()] = className
        self._tests[className][result_type] += 1

    def _writeCount(self, count, style):
        """Writes the summary count for the given class and result type"""
        if style is not None and count > 0:
            self._strbuf.write('<td class="{}">{:d}</td>'.format(style, count))
        else:
            self._strbuf.write('<td>{:d}</td>'.format(count))

    def addError(self, test, err):
        """Called when an error has occurred.
        'err' is a tuple of values as returned by sys.exc_info()."""
        if isinstance(test, TestCase):
            self._incrementCount(test, 'error')
        else:
            # Fixture errors generate a unittest._ErrorHolder object
            moduleName, className, methodName = self._errorHolderInfo(test)
            self._tests[className] = {'pass': 0, 'error': 1, 'fail': 0, 'upass' : 0}
            self._classes['0'] = className

    def addFailure(self, test, err):
        """Called when an error has occurred.
        'err' is a tuple of values as returned by sys.exc_info()."""
        self._incrementCount(test, 'fail')

    def addSuccess(self, test):
        """Called when a test has completed successfully"""
        self._incrementCount(test, 'pass')

    def addUnexpectedSuccess(self, test):
        """Called when a test was expected to fail, but succeed."""
        self._incrementCount(test, 'upass')

    def stopTestRun(self, epoch, result):
        """Called once after all tests are executed."""
        self._strbuf.write('<table>\n')
        self._strbuf.write('<tr><th>Class</th><th>Passed</th><th>Error</th><th>Failed</th></tr>\n')
        for classIdentifier in sorted(self._classes.keys(), key=CompareTestIds):
            className = self._classes[classIdentifier]
            self._strbuf.write('<tr><td><a href="details.html#' + className + '">' + className + '</a></td>')
            passed = self._tests[className]['pass']
            errors = self._tests[className]['error']
            failed = self._tests[className]['fail']
            if errors != 0 or failed != 0:
                self._writeCount(passed, None)
            else:
                self._writeCount(passed, 'pass')
            self._writeCount(errors, 'error')
            self._writeCount(failed, 'fail')
            self._strbuf.write('</tr>\n')
        self._strbuf.write('</table>\n')
        super().stopTestRun(epoch, result)


class HtmlTestDetailsReporter(HtmlTestReporterBase):
    """A test result class that can write the test result details to a file in XHTML format."""

    _result_lookup = {'skip' : 'Skipped', 'pass' : 'Pass', 'error' : 'Error', 'fail' : 'Failure', \
        'efail' : 'Expected Failure', 'upass' : 'Unexpected Success'}

    def __init__(self, filename):
        super().__init__('Test Details', filename)
        self._classes = {}
        self._tests = {}

    def _addResult(self, test, resultClass, reason):
        """Add a test result verdict"""
        className = test.__class__.__name__
        testName = test.getTestMethodName()
        testIdentifier = test.getTestIdentifier()
        if className in self._tests:
            self._tests[className][testIdentifier] = (testName, resultClass, reason)
        else:
            self._tests[className] = {testIdentifier: (testName, resultClass, reason)}
            self._classes[testIdentifier] = className

    def _writeDetails(self, firstRow, rowSpan, className, testIdentifier, testName, resultClass, resultText, reason):
        if reason is None:
            reason = '&nbsp;'
        if firstRow:
            self._strbuf.write( \
                '<tr><td rowspan="{}"><a name="{}">{}</a></td><td>{}</td><td>{}</td><td class="{}">{}</td><td><pre>{}</pre></td></tr>'. \
                format(rowSpan, className, className, testIdentifier, testName, resultClass, resultText, reason))
        else:
            self._strbuf.write('<tr><td>{}</td><td>{}</td><td class="{}">{}</td><td><pre>{}</pre></td></tr>'.format( \
                testIdentifier, testName, resultClass, resultText, reason))

    def addError(self, test, err):
        """Called when an error has occurred.
        'err' is a tuple of values as returned by sys.exc_info()."""
        reason = self._excInfoString(err, test)
        if isinstance(test, TestCase):
            self._addResult(test, 'error', reason)
        else:
            # Fixture errors generate a unittest._ErrorHolder object
            moduleName, className, methodName = self._errorHolderInfo(test)
            self._tests[className] = {'0': (methodName, 'error', reason)}
            self._classes['0'] = className

    def addExpectedFailure(self, test, err):
        """Called when an expected failure/error occured."""
        self._addResult(test, 'efail', self._excInfoString(err, test))

    def addFailure(self, test, err):
        """Called when an error has occurred.
        'err' is a tuple of values as returned by sys.exc_info()."""
        self._addResult(test, 'fail', self._excInfoString(err, test))

    def addSkip(self, test, reason):
        """Called when a test is skipped."""
        self._addResult(test, 'skip', reason)

    def addSuccess(self, test):
        """Called when a test has completed successfully"""
        self._addResult(test, 'pass', None)

    def addUnexpectedSuccess(self, test):
        """Called when a test was expected to fail, but succeed."""
        self._addResult(test, 'upass', None)

    def stopTestRun(self, epoch, result):
        """Called once after all tests are executed."""
        self._strbuf.write('<table>\n')
        self._strbuf.write('<tr><th align="left">Test Class</a></th><th>Test ID</th>' +
            '<th>Test Name</th><th>Test Verdict</th><th>Failure Cause</th></tr>\n')
        for classIdentifier in sorted(self._classes.keys(), key=CompareTestIds):
            className = self._classes[classIdentifier]
            testEntry = self._tests[className]
            rowSpan = len(testEntry)
            firstRow = True
            for testIdentifier in sorted(testEntry.keys(), key=CompareTestIds):
                testName, resultClass, reason = testEntry[testIdentifier]
                resultText = self._result_lookup[resultClass]
                self._writeDetails(firstRow, rowSpan, className, testIdentifier, testName, resultClass, resultText, reason)
                firstRow = False
        self._strbuf.write('</table>\n')
        super().stopTestRun(epoch, result)


class LogTestReporter(ITestResultListener):
    """A test result class that can report test results to a logger."""

    def __init__(self):
        super().__init__()
        self._logger = logging.getLogger(self.__class__.__name__)

    def _logTestDuration(self, result):
        """Prints the test duration"""
        timeTaken = self._stopDateTime - self._startDateTime # operator returns a timedelta object
        timeTakenMins = int(timeTaken.seconds / 60) # microsecond resolution
        timeTakenSecs = int(timeTaken.seconds - (timeTakenMins * 60))
        tests_run = result.getTestsRun()
        self._logger.info('Ran {} test{} in {}m {}s'.format(tests_run,
            tests_run != 1 and 's' or '', timeTakenMins, timeTakenSecs))

    def _logTestErrors(self, result):
        """Print the errors in the test results."""
        # print the error categories in order of importance (most important first)
        self._logTestErrorList('TEST ERROR', result.getErrors())
        self._logTestErrorList('TEST FAILURE', result.getFailures())

    def _logTestErrorList(self, flavour, errors):
        if len(errors) > 0:
            self._logger.info('################ {}S ################'.format(flavour))
        for test, err in errors:
            self._logger.info('{}: {}\n{}'.format(flavour, test, err))

    def _logVerdict(self, result):
        """Prints the test verdict."""
        results = map(len, (result.getExpectedFailures(),
                            result.getUnexpectedSuccesses(),
                            result.getSkipped()))
        expectedFailures, unexpectedSuccesses, skipped = results
        infos = []
        if not result.wasSuccessful():
            verdict = 'FAIL'
            failed, errored = len(result.getFailures()), len(result.getErrors())
            if failed:
                infos.append('failures={}'.format(failed))
            if errored:
                infos.append('errors={}'.format(errored))
        else:
            verdict = 'PASS'
        if skipped:
            infos.append('skipped={}'.format(skipped))
        if expectedFailures:
            infos.append('expected failures={}'.format(expectedFailures))
        if unexpectedSuccesses:
            infos.append('unexpected successes={}'.format(unexpectedSuccesses))
        if infos:
            self._logger.info('VERDICT: {} ({})'.format(verdict, ', '.join(infos)))
        else:
            self._logger.info('VERDICT: {}'.format(verdict))

    def _logSummary(self, result):
        """ Print the test results summary."""
        testTypes = ((result.getSkipped(), 'Skipped'),
                    (result.getSuccesses(), 'Passed'),
                    (result.getErrors(), 'Errors'),
                    (result.getFailures(), 'Failures'),
                    (result.getExpectedFailures(), 'Expected Failures'),
                    (result.getUnexpectedSuccesses(), 'Unexpected Successes')
        )

        # Count the different test categories.  By initialising certain counts we ensure that
        # those counts will always be printed out even if there are no tests in that
        # catgeory.
        counts = {'Failures':0, 'Errors':0, 'Passed':0}

        for list, title in testTypes:
            if len(list) > 0:
                self._logger.info('')
                self._logger.info('{}: '.format(title))
                for test in list:
                    # Sadly the library holds unexpected successes in a different way
                    # to everything else.
                    if title in ('Passed', 'Unexpected Successes'):
                        testObject = test
                    else:
                        testObject = test[0]

                    # Print out the test and increment the count of tests with this status.
                    if isinstance(testObject, TestCase):
                        self._logger.info('    {}'.format(testObject.getTestDescription()))
                    else:
                        # fixture errors are ErrorHolder objects
                        self._logger.info('    {}'.format(testObject))
                    if title in counts:
                        counts[title] += 1
                    else:
                        counts[title] = 1

        # Print the counts
        self._logger.info('')
        self._logger.info('Counts:')
        for list, title in testTypes:
            if title in counts:
                self._logger.info('    {:21s} {}'.format(title+':', counts[title]))

    def startTestRun(self, epoch, fixture, suite):
        """Called once before any tests are executed."""
        self._startDateTime = epoch

    def stopTestRun(self, epoch, result):
        """Called once after all tests are executed."""
        self._stopDateTime = epoch
        self._logTestErrors(result)
        self._logger.info('-' * 70)
        self._logTestDuration(result)
        self._logVerdict(result)
        self._logSummary(result)


class TextTestReporter(ITestResultListener):
    """A test result class that can print formatted text results to a stream."""

    _separator1 = '=' * 70
    _separator2 = '-' * 70
    _separator3 = '#' * 70

    def __init__(self, stream, descriptions, verbosity):
        super().__init__()
        self._stream = WritelnDecorator(stream)
        self._showAll = verbosity > 1
        self._dots = verbosity == 1
        self._descriptions = descriptions

    def _getDescription(self, test):
        if self._descriptions:
            return test.shortDescription() or str(test)
        else:
            return str(test)

    def _printTestDuration(self, result):
        """Prints the test duration"""
        timeTaken = self._stopDateTime - self._startDateTime # operator returns a timedelta object
        timeTakenMins = int(timeTaken.seconds / 60) # microsecond resolution
        timeTakenSecs = int(timeTaken.seconds - (timeTakenMins * 60))
        tests_run = result.getTestsRun()
        self._stream.writeln('Ran {} test{} in {}m {}s'.format(tests_run,
            tests_run != 1 and 's' or '', timeTakenMins, timeTakenSecs))

    def _printTestErrors(self, result):
        """Print the errors in the test results."""
        # print the error categories in order of importance (most important first)
        self._printTestErrorList('ERROR', result.getErrors())
        self._printTestErrorList('FAIL', result.getFailures())

    def _printTestErrorList(self, flavour, errors):
        for test, err in errors:
            self._stream.writeln(self._separator1)
            self._stream.writeln('{}: {}'.format(flavour, self._getDescription(test)))
            self._stream.writeln(self._separator2)
            self._stream.writeln(err)

    def _printVerdict(self, result):
        """Prints the test verdict."""
        results = map(len, (result.getExpectedFailures(),
                            result.getUnexpectedSuccesses(),
                            result.getSkipped()))
        expectedFailures, unexpectedSuccesses, skipped = results
        infos = []
        if not result.wasSuccessful():
            verdict = 'FAIL'
            failed, errored = len(result.getFailures()), len(result.getErrors())
            if failed:
                infos.append('failures={}'.format(failed))
            if errored:
                infos.append('errors={}'.format(errored))
        else:
            verdict = 'PASS'
        if skipped:
            infos.append('skipped={}'.format(skipped))
        if expectedFailures:
            infos.append('expected failures={}'.format(expectedFailures))
        if unexpectedSuccesses:
            infos.append('unexpected successes={}'.format(unexpectedSuccesses))
        if infos:
            self._stream.writeln('VERDICT: {} ({})'.format(verdict, ', '.join(infos)))
        else:
            self._stream.writeln('VERDICT: {}'.format(verdict))

    def _printSummary(self, result):
        """ Print the test results summary."""
        testTypes = ((result.getSkipped(), 'Skipped'),
                    (result.getSuccesses(), 'Passed'),
                    (result.getErrors(), 'Errors'),
                    (result.getFailures(), 'Failures'),
                    (result.getExpectedFailures(), 'Expected Failures'),
                    (result.getUnexpectedSuccesses(), 'Unexpected Successes')
        )

        # Count the different test categories.  By initialising certain counts we ensure that
        # those counts will always be printed out even if there are no tests in that
        # catgeory.
        counts = {'Failures':0, 'Errors':0, 'Passed':0}

        for list, title in testTypes:
            if len(list) > 0:
                self._stream.writeln()
                self._stream.writeln('{}: '.format(title))
                for test in list:
                    # Sadly the library holds unexpected successes in a different way
                    # to everything else.
                    if title in ('Passed', 'Unexpected Successes'):
                        testObject = test
                    else:
                        testObject = test[0]

                    # Print out the test and increment the count of tests with this status.
                    if isinstance(testObject, TestCase):
                        self._stream.writeln('    {}'.format(testObject.getTestDescription()))
                    else:
                        # fixture errors are ErrorHolder objects
                        self._stream.writeln('    {}'.format(testObject))
                    if title in counts:
                        counts[title] += 1
                    else:
                        counts[title] = 1

        # Print the counts
        self._stream.writeln()
        self._stream.writeln('Counts:')
        for list, title in testTypes:
            if title in counts:
                self._stream.writeln('    {:21s} {}'.format(title+':', counts[title]))

    def addError(self, test, err):
        """Called when an error has occurred.
        'err' is a tuple of values as returned by sys.exc_info()."""
        if self._showAll:
            self._stream.writeln('ERROR')
        elif self._dots:
            self._stream.write('E')
            self._stream.flush()

    def addExpectedFailure(self, test, err):
        """Called when an expected failure/error occured."""
        if self._showAll:
            self._stream.writeln('expected failure')
        elif self._dots:
            self._stream.write('x')
            self._stream.flush()

    def addFailure(self, test, err):
        """Called when an error has occurred.
        'err' is a tuple of values as returned by sys.exc_info()."""
        if self._showAll:
            self._stream.writeln('FAIL')
        elif self._dots:
            self._stream.write('F')
            self._stream.flush()

    def addSkip(self, test, reason):
        """Called when a test is skipped."""
        if self._showAll:
            self._stream.writeln('skipped {0!r}'.format(reason))
        elif self._dots:
            self._stream.write('s')
            self._stream.flush()

    def addSuccess(self, test):
        """Called when a test has completed successfully"""
        if self._showAll:
            self._stream.writeln('ok')
        elif self._dots:
            self._stream.write('.')
            self._stream.flush()

    def addUnexpectedSuccess(self, test):
        """Called when a test was expected to fail, but succeeded."""
        if self._showAll:
            self._stream.writeln('unexpected success')
        elif self._dots:
            self._stream.write('u')
            self._stream.flush()

    def startTest(self, test):
        """Called when the given test is about to be run"""
        if self._showAll:
            self._stream.write(self._getDescription(test))
            self._stream.write(' ... ')
            self._stream.flush()

    def startTestRun(self, epoch, fixture, suite):
        """Called once before any tests are executed."""
        self._startDateTime = epoch

    def stop(self):
        """Indicates that the tests should be aborted"""
        self._stream.writeln('###############  Aborting test run due to error ######################')

    def stopTest(self, test):
        """Called when the given test has been run"""
        if self._showAll:
            self._stream.writeln()

    def stopTestRun(self, epoch, result):
        """Called once after all tests are executed."""
        self._stopDateTime = epoch
        if self._dots:
            self._stream.writeln()
        if self._showAll:
            self._printTestErrors(result)
        self._stream.writeln()
        self._stream.writeln(self._separator2)
        self._printTestDuration(result)
        self._stream.writeln()
        self._printVerdict(result)
        self._stream.writeln()
        self._stream.writeln(self._separator3)
        self._printSummary(result)


class XmlTestReporterBase(ReporterBase):
    """A test result base class XML format writers."""

    def __init__(self, filename):
        super().__init__()
        self._filename = filename
        self._testElement = None
        self.document = None

    def _appendChildElement(self, parent, tagName):
        """Creates a child element with the given tag name and appends it to the parent node"""
        node = self.document.createElement(tagName)
        parent.appendChild(node)
        return node

    def _createDocument(self, qualifiedName, xmlns=None):
        """Create the document element."""
        impl = xml.dom.minidom.getDOMImplementation()

        # minidom does not appear to support namespaces properly yet so create document
        # in default namespace but apply attribute manually to trigger schema validation
        self.document = impl.createDocument(None, qualifiedName, None)
        if xmlns is not None:
            self.document.documentElement.setAttribute('xmlns', xmlns)

        # insert the stylesheet before the document element
        #node = self.document.createProcessingInstruction('xml-stylesheet', 'type="text/xsl" href="results.xsl\"')
        #self.document.insertBefore(node, self.document.documentElement)
        #node = self.document.createComment( \
        #    "\n*************************************************************************************" + \
        #    "\n*    If you are reading this, you are not taking advantage of the XML Stylesheet    *" + \
        #    "\n*    Copy the stylesheet to the same directory as this file                         *" + \
        #    "\n*************************************************************************************\n")
        #self.document.insertBefore(node, self.document.documentElement)

    def _destroyDocument(self):
        """Destroys the document element."""
        self._testElement = None
        self.document = None

    def _findFirstChildElement(self, parent, tagName):
        """Returns the first child element with the given parent and tag name (or None if it does not exist)"""
        nodeList = parent.getElementsByTagName(tagName)
        if len(nodeList) > 0:
            return nodeList[0]
        else:
            return None

    def _findFirstChildElementWithAttribute(self, parent, tagName, attrName, attrValue):
        """Returns the first child element with the given parent, tag name and attribute with matching value (or None if it does not exist)"""
        nodeList = parent.getElementsByTagName(tagName)
        for element in nodeList:
            if element.getAttribute(attrName) == attrValue:
                break
        else:
            element = None
        return element

    def _getFirstChildElement(self, parent, tagName):
        """Returns the first child element with the given parent and tag name (creates element if it does not exist)"""
        element = self._findFirstChildElement(parent, tagName)
        if element is None:
            element = self._appendChildElement(parent, tagName)
        return element

    def _getFirstChildElementWithAttribute(self, parent, tagName, attrName, attrValue):
        """Returns the first child element with the given parent, tag name and attribute value (creates element if it does not exist)"""
        element = self._findFirstChildElementWithAttribute(parent, tagName, attrName, attrValue)
        if element is None:
            element = self._appendChildElement(parent, tagName)
            element.setAttribute(attrName, attrValue)
        return element

    def _write(self):
        """Write the report file."""
        logger = logging.getLogger(self.__class__.__name__)
        logger.info('Generating report file : ' + self._filename)
        try:
            stream = open(self._filename, 'wb')
            try:
                logger.debug('Writing contents')
                stream.write(self.document.toxml(encoding='UTF-8'))
            except IOError as writeError:
                logger.warn('Error writing file : ' + str(writeError))
            finally:
                stream.close()
                logger.debug('Closed file')
        except IOError as openError:
            logger.warn('Error opening file : ' + str(openError))

    def addFailure(self, test, err):
        """Called when an error has occurred.
        'err' is a tuple of values as returned by sys.exc_info()."""
        element = self.document.createElement('failure')
        element.appendChild(self.document.createTextNode(self._excInfoString(err, test)))
        self._testElement.appendChild(element)

    def addSkip(self, test, reason):
        """Called when a test is skipped."""
        element = self.document.createElement('skipped')
        element.appendChild(self.document.createTextNode(reason))
        self._testElement.appendChild(element)


class XmlTestReporter(XmlTestReporterBase):
    """A test result class that can write text results to a file in XML format."""

    def __init__(self, filename):
        super().__init__(filename)

    def _getSuiteElement(self):
        """Get the suite element (creates element if it does not exist)"""
        return self._getFirstChildElement(self.document.documentElement, 'suite')

    def _getModuleElement(self, module_name):
        """Get the module element with the specified name (creates element if it does not exist)"""
        parent = self._getSuiteElement()
        return self._getFirstChildElementWithAttribute(parent, 'module', 'name', module_name)

    def _getClassElement(self, module_name, className):
        """Get the class element with the specified name (creates element if it does not exist)"""
        parent = self._getModuleElement(module_name)
        return self._getFirstChildElementWithAttribute(parent, 'class', 'name', className)

    def addError(self, test, err):
        """Called when an error has occurred.
        'err' is a tuple of values as returned by sys.exc_info()."""
        if isinstance(test, TestCase):
            element = self.document.createElement('error')
            element.appendChild(self.document.createTextNode(self._excInfoString(err, test)))
            self._testElement.appendChild(element)
        else:
            # Fixture errors generate a unittest._ErrorHolder object
            moduleName, className, methodName = self._errorHolderInfo(test)
            parent = self._getClassElement(moduleName, className)
            testElement = self.document.createElement('test')
            testElement.setAttribute('id', '0')
            testElement.setAttribute('name', methodName)
            parent.appendChild(testElement)
            errorElement = self.document.createElement('error')
            errorElement.appendChild(self.document.createTextNode(self._excInfoString(err, test)))
            testElement.appendChild(errorElement)

    def addExpectedFailure(self, test, err):
        """Called when an expected failure/error occured."""
        element = self.document.createElement('expectedFailure')
        element.appendChild(self.document.createTextNode(self._excInfoString(err, test)))
        self._testElement.appendChild(element)

    def addUnexpectedSuccess(self, test):
        """Called when a test was expected to fail, but succeed."""
        element = self.document.createElement('unexpectedSuccess')
        self._testElement.appendChild(element)

    def startTest(self, test):
        """Called when the given test is about to be run"""
        parent = self._getClassElement(test.__module__, test.__class__.__name__)
        self._testElement = self.document.createElement('test')
        self._testElement.setAttribute('id', test.getTestIdentifier())
        self._testElement.setAttribute('name', test.getTestMethodName())
        parent.appendChild(self._testElement)

    def startTestRun(self, epoch, fixture, suite):
        """Called once before any tests are executed."""
        self._createDocument('results', 'urn:results-schema')
        self.document.documentElement.setAttribute('startTime', epoch.isoformat())

        # add the properties
        propertiesElement = self._getFirstChildElement(self.document.documentElement, 'properties')
        properties = fixture.getProperties()
        for key, value in properties.items():
            # do not save project properties
            if key.startswith('PROJECT_'):
                continue
            propertyElement = self._appendChildElement(propertiesElement, 'property')
            propertyElement.setAttribute('name', key)
            if isinstance(value, (tuple, list)):
                # discriminate between single item lists and single value properties
                propertyElement.setAttribute('type', 'list')
            else:
                # convert single items to a tuple
                value = (value, )
            for obj in value:
                valueElement = self._appendChildElement(propertyElement, 'value')
                if not isinstance(obj, str):
                    valueElement.setAttribute('type', type(obj).__name__)
                valueElement.appendChild(self.document.createTextNode(str(obj)))

    def stopTestRun(self, epoch, result):
        """Called once after all tests are executed."""
        self.document.documentElement.setAttribute('stopTime', epoch.isoformat())
        self._write()
        self._destroyDocument()


class JUnitTestReporter(XmlTestReporterBase):
    """A test result class that can write results to a file in JUnit format."""

    def __init__(self, filename):
        super().__init__(filename)

    def addError(self, test, err):
        """Called when an error has occurred.
        'err' is a tuple of values as returned by sys.exc_info()."""
        if isinstance(test, TestCase):
            element = self.document.createElement('error')
            element.appendChild(self.document.createTextNode(self._excInfoString(err, test)))
            self._testElement.appendChild(element)
        else:
            # Fixture errors generate a unittest._ErrorHolder object
            moduleName, className, methodName = self._errorHolderInfo(test)
            parent = self.document.documentElement
            testElement = self.document.createElement('testcase')
            testElement.setAttribute('name', methodName)
            testElement.setAttribute('classname', '{}.{}'.format(moduleName, className))
            parent.appendChild(testElement)
            errorElement = self.document.createElement('error')
            errorElement.appendChild(self.document.createTextNode(self._excInfoString(err, test)))
            testElement.appendChild(errorElement)

    def startTest(self, test):
        """Called when the given test is about to be run"""
        parent = self.document.documentElement
        self._testElement = self.document.createElement('testcase')
        self._testElement.setAttribute('name', test.getTestMethodName())
        self._testElement.setAttribute('classname', '{}.{}'.format(test.__module__, test.__class__.__name__))
        parent.appendChild(self._testElement)

    def startTestRun(self, epoch, fixture, suite):
        """Called once before any tests are executed."""
        self._createDocument('testsuite')
        self.document.documentElement.setAttribute('name', 'pyfwtest')

    def stopTestRun(self, epoch, result):
        """Called once after all tests are executed."""
        self.document.documentElement.setAttribute('tests', str(result.getTestsRun()))
        self._write()
        self._destroyDocument()


