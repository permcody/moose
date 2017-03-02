"""ClipperFilterBase"""
import vtk
import mooseutils
from .. import base

class ChiggerFilterBase(base.ChiggerObject):
    """
    Wrapper for creating vtk filters that can be automatically connected by ChiggerFilterSourceBase.
    """
    @staticmethod
    def getOptions():
        opt = base.ChiggerObject.getOptions()
        return opt

    # The required filter type
    VTKFILTER_TYPE = vtk.vtkAlgorithm

    def __init__(self, vtkfilter_type=None, **kwargs):
        super(ChiggerFilterBase, self).__init__(**kwargs)

        self._source = None # see initializeFilter
        self._vtkfilter = vtkfilter_type()
        if not isinstance(self._vtkfilter, self.VTKFILTER_TYPE):
            raise mooseutils.MooseException('The supplied filter is a {} but must be a {} type.'.format(vtkfilter_type.__name__, self.VTKFILTER_TYPE.__name__))

    def getVTKFilter(self):
        """
        Return the VTK object containing the filter.
        """
        return self._vtkfilter

    def initializeFilter(self, source):
        """
        An initialize method called by the source object that is connecting the vtk objects.
        """
        self._source = source
