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
# Test runner for firmware tests
####################################################################################
import argparse
import logging
import os
import sys

# work out the project and working copy root directories
__projroot = os.path.dirname(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))

# prepend library paths to the search path
sys.path.append(os.path.join(__projroot, 'contrib', 'pyfwtest'))
sys.path.append(os.path.join(__projroot, 'test', 'scripts'))
sys.path.append(os.path.join(__projroot, 'test', 'src'))
sys.path.append(os.path.join(__projroot, 'scripts'))

from pyfwtest import Properties, TestCampaignLoader
from pyfwtest import TestRunner as TestRunnerBase

class TestRunner(TestRunnerBase):
    """A custom test runner class which notifies test reporters."""

    # override method
    def _createReporters(self, result):
        """Factory method to allow derived classes to create their own reporters."""
        # Turn off the default text reporter because the log reporter will be used to
        # output formatted messages to the console for vManager filter to harvest.
        super()._createReporters(result, None)

    @classmethod
    def addProjectProperties(cls, properties, outputDirectory):
        """Adds the project properties to the test runner properties."""
        projectRootDirectory = os.path.dirname(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
        projectSensorFile = os.path.join(projectRootDirectory, 'contrib', 'target', '2v0', 'AR0825AT-REV2-DEV.xsdat')
        properties['PROJECT_ROOT_DIR'] = projectRootDirectory
        properties['PROJECT_TEST_DIR'] = os.path.join(projectRootDirectory, 'test', 'src')
        properties['PROJECT_OUTPUT_DIR'] = outputDirectory
        properties['PROJECT_SENSOR_FILE'] = projectSensorFile

        # Define the framework properties (if not already set)
        if 'FIXTURE_CLASSPATH' not in properties:
            properties['FIXTURE_CLASSPATH'] = 'test_fixture.DevwareTestFixture'
        if 'SUITE_LOADER_DEBUG' not in properties:
            properties['SUITE_LOADER_DEBUG'] = 2
        if 'RUNNER_MODE_STOP_ON_ERROR' not in properties:
            properties['RUNNER_MODE_STOP_ON_ERROR'] = False
        if 'RUNNER_MODE_DESCRIPTIONS' not in properties:
            properties['RUNNER_MODE_DESCRIPTIONS'] = 1
        if 'RUNNER_MODE_VERBOSITY' not in properties:
            properties['RUNNER_MODE_VERBOSITY'] = 2

    @classmethod
    def runCampaign(cls, outputDirectory, campaign):
        """Run a test campaign."""
        properties = campaign.getProperties()
        cls.addProjectProperties(properties, outputDirectory)
        return cls.run(campaign.getTests(), campaign.getClasses(), properties)

    @classmethod
    def runTests(cls, outputDirectory, tests=None, classes=None, stopOnError=False,
            skip=None, execute=None, descriptions=1, verbosity=2):
        """Convenience method for invoking a test run."""

        # Define the framework properties
        properties = Properties({
            'SUITE_LOADER_DEBUG'        : min(verbosity, 3),
            'RUNNER_MODE_STOP_ON_ERROR' : stopOnError,
            'RUNNER_MODE_DESCRIPTIONS'  : descriptions,
            'RUNNER_MODE_VERBOSITY'     : verbosity,
        })
        if skip is not None:
            properties['SUITE_FILTER_EXCLUDES'] = skip
        if execute is not None:
            properties['SUITE_FILTER_INCLUDES'] = execute
        cls.addProjectProperties(properties, outputDirectory)

        return cls.run(tests, classes, properties)


if __name__ == "__main__":
    def parse_model_args(args, properties):
        """Parse the virtual prototype model sub-command arguments."""
        properties['FIXTURE_CLASSPATH'] = 'test_fixture.ModelTestFixture'
        properties['FIXTURE_TOKEN_SERVER_HOST'] = args.hostname
        properties['FIXTURE_TOKEN_SERVER_PORT'] = args.port
        properties['FIXTURE_TOKEN_SERVER_TIMEOUT'] = args.timeout
        properties['FIXTURE_TEST_ACCESS_TOKEN'] = args.token

    def parse_sim_args(args, properties):
        """Parse the RTL simulator sub-command arguments."""
        properties['FIXTURE_CLASSPATH'] = 'test_fixture.SimulatorTestFixture'
        properties['FIXTURE_TOKEN_SERVER_HOST'] = args.hostname
        properties['FIXTURE_TOKEN_SERVER_PORT'] = args.port
        properties['FIXTURE_TOKEN_SERVER_TIMEOUT'] = args.timeout
        properties['FIXTURE_TEST_ACCESS_TOKEN'] = args.token

    def parse_devware_args(args, properties):
        """Parse the DevWare sub-command arguments."""
        properties['FIXTURE_CLASSPATH'] = 'test_fixture.DevwareTestFixture'

    def parse_migmate_args(args, properties):
        """Parse the MigMate sub-command arguments."""
        properties['FIXTURE_CLASSPATH'] = 'test_fixture.MigmateTestFixture'

    # Create the argument parser
    parser = argparse.ArgumentParser(prog='test_runner',
        description='Runs a suite of tests on the specified fixture')
    parser.add_argument('-o', '--output', required=True, help='Output directory')
    parser.add_argument('-s', '--stop', default=False, action='store_true', help='Stop on first errorList of test classes')
    parser.add_argument('-e', '--excludes', help='Comma separated list of tests groups to exclude')
    parser.add_argument('-i', '--includes', help='Comma separated list of tests groups to include')
    parser.add_argument('-d', '--descriptions', default=1, type=int, help='Level of test case description')
    parser.add_argument('-v', '--verbosity', default=2, type=int, help='Level of test runner verbosity')
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('-t', '--tests', help='Comma separated list of fully qualified test specifiers')
    group.add_argument('-c', '--campaign', help='Test campaign XML file')
    subparsers = parser.add_subparsers()
    # Define model fixture sub-command
    model_parser = subparsers.add_parser('model', help='Start virtual prototype model test fixture.')
    model_parser.add_argument('-n', '--hostname', default='gbbr01-jenkins', help='Token server host name')
    model_parser.add_argument('-p', '--port', type=int, default=9080, help='Token server port number')
    model_parser.add_argument('-m', '--timeout', type=int, default=60, help='Token server timeout in seconds')
    model_parser.add_argument('-k', '--token', required=True, help='Test access token')
    model_parser.set_defaults(func=parse_model_args)
    # Define Simulator fixture sub-command
    sim_parser = subparsers.add_parser('sim', help='Start RTL simulator test fixture.')
    sim_parser.add_argument('-n', '--hostname', default='gbbr01-jenkins', help='Token server host name')
    sim_parser.add_argument('-p', '--port', type=int, default=9080, help='Token server port number')
    sim_parser.add_argument('-m', '--timeout', type=int, default=120, help='Token server timeout in seconds')
    sim_parser.add_argument('-k', '--token', required=True, help='Test access token')
    sim_parser.set_defaults(func=parse_sim_args)
    # Define DevWare fixture sub-command
    devware_parser = subparsers.add_parser('devware', help='Start DevWare test fixture.')
    devware_parser.set_defaults(func=parse_devware_args)
    # Define MigMate fixture sub-command
    migmate_parser = subparsers.add_parser('migmate', help='Start MigMate test fixture.')
    migmate_parser.set_defaults(func=parse_migmate_args)

    # Parse the command line arguments
    args = parser.parse_args()

    # Create the output directory if it does not already exist
    if not os.path.exists(args.output):
        os.makedirs(args.output)

    # Set root logger level
    rootLogger = logging.getLogger()
    rootLogger.setLevel(logging.DEBUG)

    # Configure the logger to output info level to the console
    consoleHandler = logging.StreamHandler(sys.stdout)
    consoleHandler.setFormatter(logging.Formatter('%(name)s [%(levelname)s] %(message)s'))
    if args.verbosity > 2:
        consoleHandler.setLevel(logging.DEBUG)
    else:
        consoleHandler.setLevel(logging.INFO)    
    rootLogger.addHandler(consoleHandler)

    # Configure the logger to output debug level to a file
    logFileHandler = logging.FileHandler(os.path.join(args.output, 'pyfwtest.log'), mode='w')
    logFileHandler.setFormatter(logging.Formatter('%(asctime)s %(name)s [%(levelname)s] %(message)s'))
    logFileHandler.setLevel(logging.DEBUG)
    rootLogger.addHandler(logFileHandler)

    # If running a campaign file
    if args.campaign:
        # Parse the campaign file
        campaign = TestCampaignLoader().load(args.campaign)
        # Get the campaign properties
        properties = campaign.getProperties()
        # Get the campaign tests
        tests = campaign.getTests()
        classes = campaign.getClasses()
    # Else running a list of tests
    else:
        # Parse the properties
        properties = Properties({
            'SUITE_LOADER_DEBUG'        : min(args.verbosity, 3),
            'RUNNER_MODE_STOP_ON_ERROR' : args.stop,
            'RUNNER_MODE_DESCRIPTIONS'  : args.descriptions,
            'RUNNER_MODE_VERBOSITY'     : args.verbosity,
        })
        # Parse the sub-command options
        args.func(args, properties)
        # Parse optional command line arguments
        if args.excludes is not None:
            properties['SUITE_FILTER_EXCLUDES'] = args.excludes
        if args.includes is not None:
            properties['SUITE_FILTER_INCLUDES'] = args.includes
        # Parse the comma separated list of fully qualified test specifiers
        # into the list of test method names and the list of test classes
        tests = []
        classes = []
        specifier_list = args.tests.split(',')
        for specifier in specifier_list:
            fields = specifier.split('.')
            classes.append('{}.{}'.format(fields[0], fields[1]))
            if len(fields) > 2:
                tests.append(fields[2])

    # Add the project specific properties
    TestRunner.addProjectProperties(properties, args.output)

    # Launch the runner
    result = TestRunner.run(tests, classes, properties)
    if result is None:
        print('Failed to execute test suite', file=sys.stderr)
        sys.exit(1)

