/*******************************************************************************
 * Copyright (c) 2006 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors:
 *   David Knibb               initial implementation      
 *   Matthew Webster           Eclipse 3.2 changes     
 *******************************************************************************/

package org.eclipse.equinox.weaving.hooks;

import java.io.IOException;

import org.eclipse.equinox.weaving.adaptors.IAspectJAdaptor;
import org.eclipse.osgi.baseadaptor.bundlefile.BundleFile;

public class BaseAjBundleFile extends AspectJBundleFile {

	public BaseAjBundleFile (IAspectJAdaptor aspectjAdaptor, BundleFile bundleFile) throws IOException {
		super(aspectjAdaptor,bundleFile);
	}
}
