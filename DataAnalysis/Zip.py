from FolderIO import InputFromFolder
from pax.datastructure import EventProxy
import zipfile


class ReadZipped(InputFromFolder):
    """Read a folder of zipfiles containing [some format]
    Should be followed by a decoder plugin who will decompress and decode the events.
    It's better to split this task up, since input is single-core only,
    while encoding & compressing can still be done by the processing workers
    """
    do_output_check = False
    file_extension = 'zip'

    def open(self, filename):
        self.current_file = zipfile.ZipFile(filename)
        self.event_numbers = sorted([int(x)
                                     for x in self.current_file.namelist()])

    def get_event_numbers_in_current_file(self):
        return self.event_numbers

    def get_single_event_in_current_file(self, event_number):
        with self.current_file.open(str(event_number)) as event_file_in_zip:
            data = event_file_in_zip.read()
            return EventProxy(data=data, block_id=-1, event_number=event_number)

    def close(self):
        """Close the currently open file"""
        self.current_file.close()


