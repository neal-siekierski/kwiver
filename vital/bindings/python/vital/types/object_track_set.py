"""
ckwg +31
Copyright 2017 by Kitware, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * Neither name of Kitware, Inc. nor the names of any contributors may be used
   to endorse or promote products derived from this software without specific
   prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

==============================================================================

Interface to VITAL object track class.

"""
import ctypes

from vital.types import (
    TrackState,
    DetectedObject
)
from vital.util import VitalObject, free_void_ptr


class ObjectTrackState( TrackState ):
    """
    vital::track::feature_track_state interface class
    """
    def __init__(self, frame, detection=None, from_cptr=None):
        """
        Initialize new track state

        :param detection: Optional DetectedObject instance associated with this state.
        :type detection: vital.types.DetectedObject
        """
        super(ObjectTrackState, self).__init__(from_cptr, frame, detection)

    def _new(self, frame, detection):
        """
        :param detection: Optional DetectedObject instance associated with this state.
        :type detection: vital.types.DetectedObject
        """
        return self._call_cfunc(
            "vital_object_track_state_new",
            [ctypes.c_int64, DetectedObject.c_ptr_type()],
            [frame, detection],
            self.C_TYPE_PTR
        )

    def _destroy(self):
        self._call_cfunc(
            "vital_track_state_destroy",
            [self.C_TYPE_PTR],
            [self],
        )

    @property
    def detection(self):
        d_ptr = self._call_cfunc(
            "vital_object_track_state_detection",
            [self.C_TYPE_PTR],
            [self],
            DetectedObject.c_ptr_type()
        )
        # f_ptr may be null
        if d_ptr:
            return DetectedObject(from_cptr=d_ptr)
        else:
            return None

class ObjectTrackSet( TrackSet ):
    """
    vital::track::object_track_set interface class
    """
    def _new(self, track_list):
        """
        :param track_list: List of tracks to initialize the set with
        :type track_list: collections.Iterable[Track] | None
        """
        if track_list is None:
            track_list = []
        # noinspection PyCallingNonCallable
        c_track_array = (Track.c_ptr_type() * len(track_list))(
            *(t.c_pointer for t in track_list)
        )

        return self._call_cfunc(
            'vital_object_trackset_new',
            [ctypes.c_size_t, ctypes.POINTER(Track.c_ptr_type())],
            [len(track_list), c_track_array],
            self.C_TYPE_PTR
        )
