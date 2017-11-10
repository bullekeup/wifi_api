/* mem.h -- Memory barriers used in MSL-common/list.h
 *
 * Copyright (C) 2017 Axel Moinet (BuLLeKeUp) <bullekeup@gmail.com>
 *
 * This file is part of MSL (https://bullekeup.github.io/MSL).
 *
 * MSL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with MSL.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __MSL_COMMON_MEM_H_
#define __MSL_COMMON_MEM_H_

/* TODO: Write REAL memory barriers (a bit like thoses of the kernel) */
#define WRITE_ONCE(x, val) x = (val)
#define READ_ONCE(x) x

#endif /* LIB_NETUTILS_MEM_H_ */
