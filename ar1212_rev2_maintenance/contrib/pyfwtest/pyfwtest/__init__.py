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
# Firmware test framework package
####################################################################################

"""A package which enhances unittest to provide XML test campaigns, XML test results and XHTML test reports."""

__all__ = ['TestCase', 'testIdentifier', 'testGroups', 'skip', 'skipIf', 'skipUnless', 'expectedFailure']

from unittest import skip, skipIf, skipUnless, expectedFailure

from .campaign import TestCampaign
from .campaign_loader import TestCampaignLoader, FailedTestCampaignLoader
from .case import TestCase
from .decorators import testIdentifier, testGroups
from .fixture import TestFixture
from .listeners import ITestRunnerListener, ITestResultListener
from .properties import Properties
from .reporter import ReporterBase, HtmlTestReporterBase, HtmlTestSummaryReporter, \
    HtmlTestDetailsReporter, JUnitTestReporter, LogTestReporter, TextTestReporter, \
    XmlTestReporterBase, XmlTestReporter
from .result import TestResultNotifier
from .runner import TestRunner
from .suite_loader import TestSuiteLoader

