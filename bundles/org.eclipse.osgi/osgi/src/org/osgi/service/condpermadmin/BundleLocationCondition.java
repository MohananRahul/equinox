/*
 * $Header: /cvshome/build/org.osgi.service.condpermadmin/src/org/osgi/service/condpermadmin/BundleLocationCondition.java,v 1.14 2005/08/03 21:18:43 twatson Exp $
 * 
 * Copyright (c) OSGi Alliance (2005). All Rights Reserved.
 * 
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v1.0 which accompanies this 
 * distribution, and is available at http://www.eclipse.org/legal/epl-v10.html.
 */
package org.osgi.service.condpermadmin;

import java.security.AccessController;
import java.security.PrivilegedAction;
import java.util.Hashtable;

import org.osgi.framework.*;

/**
 * 
 * Checks to see if a Bundle matches the given location pattern. Pattern
 * matching is done according to the filter string matching rules.
 * 
 * @version $Revision: 1.14 $
 */
public class BundleLocationCondition {
	private static final String	CONDITION_TYPE	= "org.osgi.service.condpermadmin.BundleLocationCondition";

	/**
	 * Constructs a condition that tries to match the passed Bundle's location
	 * to the location pattern.
	 * 
	 * @param bundle the Bundle being evaluated.
	 * @param info the ConditionInfo to construct the condition for. The args of
	 *        the ConditionInfo specify the location to match the Bundle
	 *        location to. Matching is done according to the filter string 
	 *        matching rules.  Any '*' characters in the location argument are
	 *        used as wildcards when matching bundle locations unless they are
	 *        escaped with a '\' character.
	 */
	static public Condition getCondition(final Bundle bundle, ConditionInfo info) {
		if (!CONDITION_TYPE.equals(info.getType()))
			throw new IllegalArgumentException(
					"ConditionInfo must be of type \"" + CONDITION_TYPE + "\"");
		String[] args = info.getArgs();
		if (args.length != 1)
			throw new IllegalArgumentException("Illegal number of args: "
					+ args.length);
		String bundleLocation = (String) AccessController.doPrivileged(new PrivilegedAction() {
					public Object run() {
						return bundle.getLocation();
					}
				});
		Filter filter = null;
		try {
			filter = FrameworkUtil.createFilter("(location=" + encodeLocation(args[0]) + ")");
		}
		catch (InvalidSyntaxException e) {
			// this should never happen, but just incase
			throw new RuntimeException("Invalid filter: " + e.getFilter());
		}
		Hashtable matchProps = new Hashtable(2);
		matchProps.put("location", bundleLocation);
		return filter.match(matchProps) ? Condition.TRUE : Condition.FALSE;
	}

	private BundleLocationCondition() {
		// private constructor to prevent objects of this type
	}
	/**
	 * Encode the value string such that '(', ')'
	 * and '\' are escaped.  The '\' char is only escaped if it is not
	 * followed by a '*'.
	 *
	 * @param value unencoded value string.
	 * @return encoded value string.
	 */
	private static String encodeLocation(String value) {
		boolean encoded = false;
		int inlen = value.length();
		int outlen = inlen << 1; /* inlen * 2 */

		char[] output = new char[outlen];
		value.getChars(0, inlen, output, inlen);

		int cursor = 0;
		for (int i = inlen; i < outlen; i++) {
			char c = output[i];
			switch (c) {
				case '\\' :
					if (i + 1 < outlen && output[i + 1] == '*')
						break;
				case '(' :
				case ')' :
					output[cursor] = '\\';
					cursor++;
					encoded = true;
					break;
			}

			output[cursor] = c;
			cursor++;
		}

		return encoded ? new String(output, 0, cursor) : value;
	}
}
