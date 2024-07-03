import { Component, OnInit, Input } from '@angular/core';
import { Router } from '@angular/router';
import { Trip } from '../models/trip';
import { TripDataService } from '../services/trip-data.service';
import { AuthenticationService } from '../services/authentication.service';

@Component({
  selector: 'app-trip-card',
  templateUrl: './trip-card.component.html',
  styleUrls: ['./trip-card.component.css']
})
export class TripCardComponent implements OnInit {

  @Input('trip') trip: any;
  constructor(
    private authenticationService: AuthenticationService,
    private tripDataService: TripDataService,
    private router: Router
  ) { }

  ngOnInit() {
  }
/*  */
  public isLoggedIn(): boolean {
    return this.authenticationService.isLoggedIn();
  }

  private editTrip(trip: Trip): void {
    console.log('Inside TripListingComponent#editTrip');
    localStorage.removeItem("tripCode");
    localStorage.setItem("tripCode", trip.code);
    this.router.navigate(['edit-trip']);
  }

  private deleteTrip(tripCode: string): void {
    console.log('Inside TripCardComponent#deleteTrip');
    this.tripDataService.deleteTrip(tripCode)
      .then(response => {
        console.log(response); // Log the response if needed
      })
      .catch(error => {
        console.error('Error deleting trip:', error);
        // Handle error
      });
  }

}
