What is the difference between "dlb_buffer" and "dlb_buffer_fixed"?
-------------------------------------------------------------------
dlb_buffer_fixed is identical to dlb_buffer, except that when being built for a
fixed point intrinsics backend, it will not include any floating point code.

Behaviour wise, this means that dlb_buffer_convert() may return
DBC_ERR_UNSUPPORTED_CONVERSION in some configurations and not for others when
using "dlb_buffer_fixed".

For floating point backends, the two projects are identical.

Naming
------
The naming of "dlb_buffer_fixed" is slightly unfortunate. The word "fixed" is
to be read as "fixed point" not "broken/fixed". But, it will still have 
floating point code in it when running on a floating point backend, so there is
some minor confusion there.

It would have been preferable to instead name the projects

"dlb_buffer_float" <- always has floating point (currently called "dlb_buffer")
"dlb_buffer"       <- has floating point only for floating point backends
                      (currently called "dlb_buffer_fixed")

However, since earlier releases only had the version which always included
floating point code, we keep the name to remain backwards compatible.
