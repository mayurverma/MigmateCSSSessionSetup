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
# Builds a Jenkins job if the Subversion revision build parameters have changed.
####################################################################################
import argparse
import os
import re
import subprocess
import sys
import urllib.request

class PropertySource():
    """Property source class."""
    def populate(self):
        """Populate the property information from the source."""
        raise NotImplementedError

    def get_property(self, name, attr):
        """Reads property value with the given name and attribute."""
        raise NotImplementedError

class SubversionRepository(PropertySource):
    """Subversion repository class."""
    def __init__(self, url):
        self._url = url
        self._properties = {}

    # override method
    def populate(self):
        """Populate the repository information from the server."""
        info = subprocess.run(['svn', 'info', self._url], stdout=subprocess.PIPE).stdout.decode('utf-8')
        #print('SubversionRepository info : {}'.format(info))
        for line in info.splitlines():
            print('SubversionRepository line : {}'.format(line))
            result = re.match(r'^([^:]+): ?(.*)', line)
            if result:
                key = result.group(1)
                value = result.group(2)
                if key == 'Revision':
                    self._properties['revision'] = value.strip()
                    print('SubversionRepository._properties[revision] : {}'.format(self._properties['revision']))
                if key == 'Last Changed Rev':
                    self._properties['last-changed-rev'] = value.strip()
                    print('SubversionRepository._properties[last-changed-rev] : {}'.format(self._properties['last-changed-rev']))
    
    # override method
    def get_property(self, name, attr):
        """Reads property value with the given name and attribute."""
        return self._properties[name]

    def get_url(self):
        """Returns the repository URL."""
        return self._url

class JenkinsBuildInfo(PropertySource):
    """Jenkins job build information."""
    def __init__(self, job, id, api='api/python'):
        self._url = '{}/{}/{}'.format(job.get_url(), id, api)
        self._properties = {'parameters' : {}, 'revisions' : {}}
        #print('JenkinsBuildInfo._url :: {}'.format(self._url))     
        #for k,v in self._properties.items():		
        #    print('JenkinsBuildInfo._properties :: {}: {}'.format(k,v))

    # override method
    def populate(self):
        """Populate the build information from the server."""
        print('populate URL : {}'.format(self._url))
        info = eval(urllib.request.urlopen(self._url).read())
        # Parse the build number
        self._properties['number'] = info['number']
        print('JenkinsBuildInfo._properties[number] : {}'.format(self._properties['number']))
        # Parse the build parameters
        #print('info : {}'.format(info['actions']))
        for action in info['actions']:
            if 'parameters' in action:
                for param in action['parameters']:
                    self._properties['parameters'][param['name']] = param['value']
                    print('self._properties[parameters][param[name]]: {}'.format(param['value']))
        # Parse the module revisions
        if 'changeSets' in info:
            print('changeSets : {}'.format(info['changeSets']))
            for changeSet in info['changeSets']:
                print('changeSet : {}'.format(info['changeSets']))
                for element in changeSet['revisions']:
                    print('element : {}'.format(element))
                    module = element['module']
                    revision = element['revision']
                    print('module : {}'.format(module))
                    print('revision : {}'.format(revision))
                    if module in self._properties['revisions']:
                        revision = max(int(self._properties['revisions'][module]), revision)
                    self._properties['revisions'][element['module']] = str(revision)
                    print('JenkinsBuildInfo._properties[revisions][element[module]] : {}'.format(self._properties['revisions'][element['module']]))
    
    # override method
    def get_property(self, name, attr):
        """Reads property value with the given name and attribute."""
        if attr is None:
            return self._properties[name]
        else:
            return self._properties[name][attr]

class JenkinsJob():
    """Jenkins job class."""
    def __init__(self, url, token=None):
        self._url = url
        self._token = token
        #print('JenkinsJob._url : {}'.format(self._url))
        #print('JenkinsJob._token : {}'.format(self._token))
    
    def _get_last_build_parameters(self, next):
        """Gets the next build parameters from the property sources."""
        last_build = self.get_last_build()
        last_build.populate()
        build_parameters = {}
        for key in next.keys():
            build_parameters[key] = last_build.get_property('parameters', key)
            print('_get_last_build_parameters build_parameters :: {} : {}'.format(key, build_parameters[key]))	
        for k,v in build_parameters.items():
            print('_get_last_build_parameters :: {} : {}'.format(k,v))		
        return build_parameters
        
    def _get_next_build_parameters(self):
        """Gets the next build parameters from the property sources."""
        build_parameters = {}
        #print('_get_next_build_parameters self._properties: {}'.format(self._properties))
        for source, mapping in self._properties:
            print('_get_next_build_parameters source: {}, mapping: {}'.format(source, mapping))
            source.populate()
            for name, element in mapping.items():
                build_parameters[name] = source.get_property(element[0], element[1])
                print('_get_next_build_parameters build_parameters :: {} : {}'.format(name, build_parameters[name]))
        for k,v in build_parameters.items():
            print('_get_next_build_parameters :: {} : {}'.format(k,v))
        return build_parameters

    def _start_build(self, next, last):
        """Determine whether to start a new build job."""
        # If the next build would contain a HEAD revision then do not build now.
        # This is caused by an upstream build that was triggered manually with a
        # HEAD revision rather than a revision number and should not be propagated.
        for name in next.keys():
            if name.endswith('_REVISION') and next[name] == 'HEAD':
                return False

        # For each build parameter
        for name in next.keys():
            print("name = {}".format(name))
            # If build parameter is a revision number
            if name.endswith('_REVISION'):
                #print("name = {}".format(name))
                # If last build was a manual HEAD revision then rebuild
                if last[name] == 'HEAD':
                    print('_start_build last[name] : {}'.format(last[name]))
                    return True
                # If revision numbers differ then rebuild
                if int(next[name]) > int(last[name]):
                    print('_start_build next[name]:{} > last[name]:{}'.format(int(next[name]), int(last[name])))
                    return True
            # Else build parameter is an ordinary string
            else:
                # If build parameter has changed then rebuild
                #next["BUILD_SELECTOR"]="15"
                #last["BUILD_SELECTOR"]=15 #"lastbuild" 
                print("type next[name]:{}, last[name]:{}".format(type(next[name]), type(last[name])))
                print('_start_build next[name]:{},  last[name]:{}'.format(next[name], last[name]))
                if str(next[name]).isnumeric() and str(last[name]).isnumeric():
                    if int(next[name]) != int(last[name]):
                        return True
                elif type(next[name]) == str and type(last[name]) == str:
                     if next[name] != last[name]:
                         return True
                else:
                    return True
					

        # Otherwise do not rebuild
        return False

    def set_build_properties(self, properties):
        """Sets the next build properties."""
        self._properties = properties

    def get_url(self):
        """Returns the job URL."""
        return self._url

    def get_last_build(self):
        """Returns the last build information."""
        return JenkinsBuildInfo(self, '/lastBuild')

    def get_last_successful_build(self):
        """Returns the last successful build information."""
        return JenkinsBuildInfo(self, '/lastSuccessfulBuild')

    def build(self):
        """Decides whether to trigger a new build."""
        next_params = self._get_next_build_parameters()
        last_params = self._get_last_build_parameters(next_params)
        print('Next build : {}'.format(next_params))
        print('Last build : {}'.format(last_params))        
        if self._start_build(next_params, last_params):
            # Generate the job URL for a new build with token and parameters
            url = '{}/buildWithParameters?'.format(self._url)
            if self._token is not None:
                url += 'token={}'.format(self._token)
            for key, value in next_params.items():
                url += '&{}={}'.format(key, value)
            # Perform the request
            print('url : {}'.format(url))
            request = urllib.request.Request(url, method='GET')
            print('{} {} {}'.format(request.method, request.full_url, request.header_items()))
            response = urllib.request.urlopen(request).read().decode('utf-8')
            if len(response) > 0:
                print(response)
            else:
                print('OK')
            
			
# Subversion repositories
svn_server = 'http://sjca-svn01.aptina.com/svn/'
repos = {
    'campaign'                 : SubversionRepository(svn_server + 'MIG/SE/AR0820_firmware/branches/ar1212_rev2_maintenance/test/scripts/TestSchedule.xlsm'),
    'firmware_trunk'           : SubversionRepository(svn_server + 'MIG/SE/AR0820_firmware/trunk'),
    'firmware_ar1212_rev2'     : SubversionRepository(svn_server + 'MIG/SE/AR0820_firmware/branches/ar1212_rev2_maintenance'),
    'model_trunk'              : SubversionRepository(svn_server + 'MIG/SE/SOC_FW_sandbox/virtual_platform_support/virtual_platforms/branches/ar1212_release/ar1212/model_build'),
    'model_ar1212_rev2'        : SubversionRepository(svn_server + 'MIG/SE/SOC_FW_sandbox/virtual_platform_support/virtual_platforms/branches/ar0820_rev2_maintenance/ar1212/model_build'),
    'aim_platform_trunk'       : SubversionRepository(svn_server + 'MIG/Sensor/aim_platform/trunk'),
    'aim_platform_ar1212_rev2_fw_testing' : SubversionRepository(svn_server + 'MIG/Sensor/aim_platform/branches/ar1212_rev2_fw_testing')
}

# Jenkins jobs
jenkins_server = 'http://innd02-jenkins:9000/'
jobs = {
    'ar1212-rev2-build'         : JenkinsJob(jenkins_server + 'job/Projects/job/Firmware/job/AR1212/job/rev2-build'),
    'ar1212-rev2-test1-model-sanity'  : JenkinsJob(jenkins_server + 'job/Projects/job/Firmware/job/AR1212/job/rev2-test1-model-sanity',   'cyw40q1r6hjg2khc'),
    'ar1212-rev2-test2-model-nightly' : JenkinsJob(jenkins_server + 'job/Projects/job/Firmware/job/AR1212/job/rev2-test2-model-nightly',  'rsn48oyae7wd9rb6'),
    'ar1212-rev2-test4-sim-sanity'    : JenkinsJob(jenkins_server + 'job/Projects/job/Firmware/job/AR1212/job/rev2-test4-sim-sanity',     'inuvq6fk29x900sa'),
    'ar1212-rev2-test5-sim-nightly'   : JenkinsJob(jenkins_server + 'job/Projects/job/Firmware/job/AR1212/job/rev2-test5-sim-nightly',    'mrufzjro7dgt6pf9'),
    'ar1212-rev2-test6-sim-weekly'    : JenkinsJob(jenkins_server + 'job/Projects/job/Firmware/job/AR1212/job/rev2-test6-sim-weekly',     'pvf7niufqodmbehg'),
}

# Define AR1212 Rev2 job build parameters
'''jobs['ar1212-rev2-test1-model-sanity'].set_build_properties(
    (
        (JenkinsBuildInfo(jobs['ar1212-rev2-build'], 'lastSuccessfulBuild'),
        {
            'BUILD_SELECTOR'    : ('number', None)
        }),
        (repos['campaign'],
        {
            'CAMPAIGN_REVISION' : ('last-changed-rev', None)
        }),
        (repos['model_ar1212_rev2'],
        {
            'MODEL_REVISION' : ('last-changed-rev', None)
        })
    ))
'''
jobs['ar1212-rev2-test4-sim-sanity'].set_build_properties(
    (
        (JenkinsBuildInfo(jobs['ar1212-rev2-build'], 'lastSuccessfulBuild'),
        {
            'BUILD_SELECTOR'    : ('number', None)
        }),
        (repos['campaign'],
        {
            'CAMPAIGN_REVISION' : ('last-changed-rev', None)
        }),
        (repos['aim_platform_ar1212_rev2_fw_testing'],
        {
            'AIM_PLATFORM_REVISION' : ('last-changed-rev', None)
        })
    ))

jobs['ar1212-rev2-test1-model-sanity'].set_build_properties(
    (
        (JenkinsBuildInfo(jobs['ar1212-rev2-test4-sim-sanity'], 'lastSuccessfulBuild'),
        {
            'BUILD_SELECTOR' : ('parameters', 'BUILD_SELECTOR'),
            'CAMPAIGN_REVISION' : ('parameters', 'CAMPAIGN_REVISION')
        }),
        (repos['model_ar1212_rev2'],
        {
            'MODEL_REVISION' : ('last-changed-rev', None)
        })
    ))

jobs['ar1212-rev2-test2-model-nightly'].set_build_properties(
    (
        (JenkinsBuildInfo(jobs['ar1212-rev2-test1-model-sanity'], 'lastSuccessfulBuild'),
        {
            'BUILD_SELECTOR' : ('parameters', 'BUILD_SELECTOR'),
            'CAMPAIGN_REVISION' : ('parameters', 'CAMPAIGN_REVISION'),
            'MODEL_REVISION' : ('parameters', 'MODEL_REVISION')
        }),
    ))

jobs['ar1212-rev2-test5-sim-nightly'].set_build_properties(
    (
        (JenkinsBuildInfo(jobs['ar1212-rev2-test4-sim-sanity'], 'lastSuccessfulBuild'),
        {
            'BUILD_SELECTOR' : ('parameters', 'BUILD_SELECTOR'),
            'CAMPAIGN_REVISION' : ('parameters', 'CAMPAIGN_REVISION'),
            'AIM_PLATFORM_REVISION' : ('parameters', 'AIM_PLATFORM_REVISION')
        }),
    ))
jobs['ar1212-rev2-test6-sim-weekly'].set_build_properties(
    (
        (JenkinsBuildInfo(jobs['ar1212-rev2-test5-sim-nightly'], 'lastSuccessfulBuild'),
        {
            'BUILD_SELECTOR' : ('parameters', 'BUILD_SELECTOR'),
            'CAMPAIGN_REVISION' : ('parameters', 'CAMPAIGN_REVISION'),
            'AIM_PLATFORM_REVISION' : ('parameters', 'AIM_PLATFORM_REVISION')
        }),
    ))

# Create the argument parser
parser = argparse.ArgumentParser(prog='jenkins_build.py',
    description='Build a Jenkins job (if the build parameters have changed).')
parser.add_argument('jobs', nargs='+', help='List of Jenkins jobs')

# Parse the command line arguments
args = parser.parse_args()

# Build each job listed on the command line
for name in args.jobs:
    jobs[name].build()
