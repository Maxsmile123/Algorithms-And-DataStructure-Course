import os
import logging

LOG_LEVEL = os.environ.get("PEQUOD_LOG_LEVEL", "INFO").upper()
logging.basicConfig(level=LOG_LEVEL,
                    format="[%(name)s] [%(levelname)s] %(message)s")
log = logging.getLogger("Pequod")
