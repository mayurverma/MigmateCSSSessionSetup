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
# Firmware test framework test campaign loader class
####################################################################################

import xml.dom
import xml.dom.minidom

from .campaign import TestCampaign

class TestCampaignLoader(object):
    """Provides the ability to load a test campaign from an XML description file"""

    def _createCampaign(self):
        """Factory method to allow derived classes to create their own test campaign."""
        return TestCampaign()

    def _filter(self, element):
        """Returns true if test element should be included in the campaign.
           Derived classes should override this to filter tests."""
        return True

    def _getPropertyValue(self, element):
        """Returns a property value."""
        value = element.firstChild.nodeValue.strip()
        if element.hasAttribute('type'):
            valueType = element.getAttribute('type')
            if valueType == 'bool':
                # cannot use bool(value) as it will always return True
                value = value.lower() not in ('false', 0)
            elif valueType == 'int':
                value = int(value)
        return value

    def _walkChildren(self, element):
        """Returns true if the parser should walk though the given elements children"""
        return element.tagName in ('campaign', 'properties', 'suite', 'module', 'class')

    def _walk(self, element):
        """Recursive method to walk through the elements of the DOM tree"""
        if self._walkChildren(element):
            # these elements have children to be processed
            for node in element.childNodes:
                if node.nodeType == xml.dom.Node.ELEMENT_NODE:
                    self._walk(node)
        elif element.tagName == 'property':
            name = element.getAttribute('name')
            values = []
            # process the property value child nodes
            for node in element.childNodes:
                if node.nodeType == xml.dom.Node.ELEMENT_NODE and node.tagName == 'value':
                    values.append(self._getPropertyValue(node))
            if len(values) == 0:
                raise xml.dom.SyntaxErr('Property has no "value" elements')
            # if the property is a list
            if element.hasAttribute('type'):
                if element.getAttribute('type') == 'list':
                    self.campaign.addProperty(name, values)
                else:
                    raise xml.dom.SyntaxErr('Unexpected type attribute value')
            # else the property is a single value
            elif len(values) > 1:
                raise xml.dom.SyntaxErr('Property is not a "list", multiple values are not allowed')
            else:
                self.campaign.addProperty(name, values[0])
        elif element.tagName == 'test' and self._filter(element):
            # found test method name
            moduleName = element.parentNode.parentNode.getAttribute('name')
            className = element.parentNode.getAttribute('name')
            methodName = element.getAttribute('name')
            self.campaign.addTest(moduleName, className, methodName)

    def load(self, filename):
        """Parse an XML test campaign file into a test campaign object"""

        # create the campaign
        self.campaign = self._createCampaign()

        # parse the file into a DOM tree
        dom = xml.dom.minidom.parse(filename)

        # now walk the tree
        self._walk(dom.documentElement)

        return self.campaign


class FailedTestCampaignLoader(TestCampaignLoader):
    """Provides the ability to load the set of last failed tests from an XML results file."""

    def _walkChildren(self, element):
        """Returns true if the parser should walk though the given elements children"""
        return element.tagName in ('results', 'properties', 'suite', 'module', 'class')

    def _filter(self, element):
        """Returns true if previous test result was an error or a failure."""
        if element.hasChildNodes():
            # find the first child element
            for node in element.childNodes:
                if node.nodeType == xml.dom.Node.ELEMENT_NODE:
                    return node.tagName in ('failure', 'error')
        return False

