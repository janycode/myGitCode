
from . import grok

from ..B import bar


from mypackage.A import grok # OK
from . import grok # OK
#import grok # error: not found
