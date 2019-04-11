!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!! viirs2ioda_vars
!! - module for variable definition to be used by multiple modules
!!   for viirs2ioda
!!
!! author: Cory Martin - cory.r.martin@noaa.gov
!! history: 2019-02-22 - original
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
module viirs2ioda_vars
  use netcdf
  use datetime_mod
  implicit none
  ! global variables provided at the command line
  character(len=255) :: gridpath,infile,outfile
  character(len=10) :: validtimestr
  type(datetime_type) :: validtime
  logical :: thinning
  ! dataset attributes
  character(len=20) :: inst, sat, tendstr
  real :: tdiff

  ! input data arrays
  integer, parameter :: ntiles_fv3=6
  integer :: nobs,nobs_out
  integer :: n_abich
  character(len=NF90_MAX_NAME), dimension(ntiles_fv3) :: fv3_gridfiles

  type viirs_aod
    !character(len=NF90_MAX_NAME) :: satellite
    !character(len=NF90_MAX_NAME) :: instrument 
    character(len=NF90_MAX_NAME) :: obstype
    real :: lat, lon
    real :: bias, uncertainty
    real, allocatable :: values(:)
    integer :: qcall,stype
  end type viirs_aod

  type(viirs_aod), allocatable, dimension(:) :: viirs_aod_input
  type(viirs_aod), allocatable, dimension(:) :: viirs_aod_output



  ! other vars/parameters
  real, parameter :: pi = acos(-1.0)
  real, parameter :: r2d = 180.0 / pi
  real, parameter :: d2r = pi / 180.
  real, parameter :: r_earth = 6378. ! km

  real, parameter :: thinning_grid_ratio_min=.75
  real, parameter :: thinning_grid_ratio_max=1.5


end module viirs2ioda_vars
