/*
 * Copyright (c) OSGi Alliance (2012, 2014). All Rights Reserved.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.osgi.service.http.runtime.dto;

import java.util.Map;
import org.osgi.dto.DTO;

/**
 * Represents the state of a Http Service Runtime.
 * 
 * @NotThreadSafe
 * @author $Id$
 */
public class RuntimeDTO extends DTO {

	/**
	 * The runtime attributes.
	 */
	public Map<String, String>	attributes;

	/**
	 * Returns the representations of the {@code javax.servlet.ServletContext} objects used by
	 * the Http service runtime. The returned array may be empty if the Http service 
	 * runtime is currently not using any {@code ServletContext} objects.
	 */
	public ServletContextDTO[] servletContextDTOs;

	/**
	 * Returns the representations of the {@code javax.servlet.ServletContext} objects
	 * currently not used by the Http service runtime due to some problem.
	 * The returned array may be empty.
	 */
	public FailedServletContextDTO[] failedServletContextDTOs;

	/**
	 * Returns the representations of the {@code javax.servlet.Servlet} servlets associated
	 * with this runtime but currently not used due to some problem.
	 * The returned array may be empty.
	 */
	public FailedServletDTO[] failedServletDTOs;

	/**
	 * Returns the representations of the resources associated with this
	 * runtime but currently not used due to some problem.
	 * The returned array may be empty.
	 */
	public FailedResourceDTO[] failedResourceDTOs;

	/**
	 * Returns the representations of the servlet {@code javax.servlet.Filter} filters
	 * associated with this runtime but currently not used due to some problem.
	 * The returned array may be empty.
	 */
	public FailedFilterDTO[] failedFilterDTOs;

	/**
	 * Returns the representations of the error page {@code javax.servlet.Servlet} servlets
	 * associated with this runtime but currently not used due to some problem.
	 * The returned array may be empty.
	 */
	public FailedErrorPageDTO[] failedErrorPageDTOs;

	/**
	 * Returns the representations of the listeners
	 * associated with this runtime but currently not used due to some problem.
	 * The returned array may be empty.
	 */
	public FailedListenerDTO[] failedListenerDTOs;
}
